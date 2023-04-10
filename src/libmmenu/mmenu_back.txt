#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <msettings.h>
#include <dlfcn.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../common/common.h"
#include "../common/defines.h"

#include "mmenu.h"

static SDL_Surface* overlay;
static SDL_Surface* screen;
static SDL_Surface* slot_overlay;
static SDL_Surface* slot_dots;
static SDL_Surface* slot_dot_selected;
static SDL_Surface* arrow;
static SDL_Surface* arrow_highlighted;
static SDL_Surface* no_preview;
static SDL_Surface* empty_slot;

enum {
	ITEM_CONTINUE,
	ITEM_SAVE,
	ITEM_LOAD,
	ITEM_OPTIONS,
	ITEM_QUIT,
};
#define MAX_ITEMS 5
#define MAX_SLOTS 8

static char* items[MAX_ITEMS];
static int slot = 0;
static int is_simple = 0;

__attribute__((constructor)) static void init(void) {
	void* librt = dlopen("librt.so.1", RTLD_LAZY | RTLD_GLOBAL); // shm
	void* libmsettings = dlopen("libmsettings.so", RTLD_LAZY | RTLD_GLOBAL);
	InitSettings();
	
	is_simple = exists(SIMPLE_MODE_PATH);
	
	items[ITEM_CONTINUE] = "Continue";
	items[ITEM_SAVE] = "Save";
	items[ITEM_LOAD] = "Load";
	items[ITEM_OPTIONS] = is_simple ? "Reset" : "Options";
	items[ITEM_QUIT] = "Quit";
	
	GFX_init();
	
	overlay = SDL_CreateRGBSurface(SDL_SWSURFACE, SCREEN_WIDTH, SCREEN_HEIGHT, 16, 0, 0, 0, 0);
	SDL_SetAlpha(overlay, SDL_SRCALPHA, 0x80);
	SDL_FillRect(overlay, NULL, 0);
	
	slot_overlay = GFX_loadImage("slot-overlay.png");
	slot_dots = GFX_loadImage("slot-dots.png");
	slot_dot_selected = GFX_loadImage("slot-dot-selected.png");
	arrow = GFX_loadImage("chevron-white.png");
	arrow_highlighted = GFX_loadImage("chevron-black.png");
	no_preview = GFX_getText("No Preview");
	empty_slot = GFX_getText("Empty Slot");
}
__attribute__((destructor)) static void quit(void) {
	SDL_FreeSurface(overlay);
	SDL_FreeSurface(slot_overlay);
	SDL_FreeSurface(slot_dots);
	SDL_FreeSurface(slot_dot_selected);
	SDL_FreeSurface(arrow);
	SDL_FreeSurface(arrow_highlighted);
	SDL_FreeSurface(no_preview);
	SDL_FreeSurface(empty_slot);
}

typedef struct __attribute__((__packed__)) uint24_t {
	uint8_t a,b,c;
} uint24_t;
static SDL_Surface* createThumbnail(SDL_Surface* src_img) {
	SDL_Surface* dst_img = SDL_CreateRGBSurface(0,SCREEN_WIDTH/2, SCREEN_HEIGHT/2,src_img->format->BitsPerPixel,src_img->format->Rmask,src_img->format->Gmask,src_img->format->Bmask,src_img->format->Amask);

	uint8_t* src_px = src_img->pixels;
	uint8_t* dst_px = dst_img->pixels;
	int step = dst_img->format->BytesPerPixel;
	int step2 = step * 2;
	int stride = src_img->pitch;
	for (int y=0; y<dst_img->h; y++) {
		for (int x=0; x<dst_img->w; x++) {
			switch(step) {
				case 1:
					*dst_px = *src_px;
					break;
				case 2:
					*(uint16_t*)dst_px = *(uint16_t*)src_px;
					break;
				case 3:
					*(uint24_t*)dst_px = *(uint24_t*)src_px;
					break;
				case 4:
					*(uint32_t*)dst_px = *(uint32_t*)src_px;
					break;
			}
			dst_px += step;
			src_px += step2;
		}
		src_px += stride;
	}

	return dst_img;
}

static int state_support = 1;
static int disable_poweroff = 0;
void ShowWarning(void) {
	state_support = 0;
	disable_poweroff = 1;
	disablePoweroff();
	
	screen = SDL_GetVideoSurface();
	GFX_ready();
	
	SDL_Surface* okay = GFX_loadImage("okay.png");
	SDL_BlitSurface(okay, NULL, screen, NULL);
	GFX_blitParagraph(screen, "This pak does not support\nsave states, auto-power off\nor quicksave and resume.", 0,0,screen->w,406);
	SDL_Flip(screen);

	SDL_Event event;
	int warned = 0;
	while (!warned) {
		while (SDL_PollEvent(&event)) {
			if (event.type==SDL_KEYDOWN) {
				SDLKey key = event.key.keysym.sym;
				if (key==BTN_A) {
					warned = 1;
					break;
				}
			}
		}
		SDL_Delay(200);
	}
	SDL_FreeSurface(okay);
}

// TODO: look at common data between SaveLoad and ShowMenu and determine which can be determined once and cached

static MenuReturnStatus SaveLoad(char* rom_path, char* save_path_template, SDL_Surface* optional_snapshot, MenuRequestState requested_state, AutoSave_t autosave) {
	int status = STATUS_CONTINUE;
	if (!state_support) return status;
	
	char *tmp;
	char rom_file[256]; // with extension
	char slot_path[256];
	char emu_name[256];
	char mmenu_dir[256];
	char txt_path[256];
	
	tmp = strrchr(rom_path,'/');
	if (tmp==NULL) tmp = rom_path;
	else tmp += 1;
	strcpy(rom_file, tmp);
	
	getEmuName(rom_path, emu_name);
	sprintf(mmenu_dir, "%s/.mmenu/%s", getenv("USERDATA_PATH"), emu_name);
	mkdir(mmenu_dir, 0755);
	
	sprintf(txt_path, "%s/%s.%d.txt", mmenu_dir, rom_file, slot);
	
	// does this game have an m3u?
	int rom_disc = -1;
	int disc = rom_disc;
	int total_discs = 0;
	char disc_name[16];
	char* disc_paths[9]; // up to 9 paths, Arc the Lad Collection is 7 discs

	// construct m3u path based on parent directory
	// essentially hasM3u() from MiniUI but we use the building blocks as well
	char m3u_path[256];
	strcpy(m3u_path, rom_path);
	tmp = strrchr(m3u_path, '/') + 1;
	tmp[0] = '\0';

	// path to parent directory
	char base_path[256]; // used below too when status==kItemSave
	strcpy(base_path, m3u_path);

	tmp = strrchr(m3u_path, '/');
	tmp[0] = '\0';

	// get parent directory name
	char dir_name[256];
	tmp = strrchr(m3u_path, '/');
	strcpy(dir_name, tmp);

	// dir_name is also our m3u file name
	tmp = m3u_path + strlen(m3u_path); 
	strcpy(tmp, dir_name);

	// add extension
	tmp = m3u_path + strlen(m3u_path);
	strcpy(tmp, ".m3u");
	
	if (exists(m3u_path)) {
		// share saves across multi-disc games
		strcpy(rom_file, dir_name);
		tmp = rom_file + strlen(rom_file);
		strcpy(tmp, ".m3u");
		
		//read m3u file
		FILE* file = fopen(m3u_path, "r");
		if (file) {
			char line[256];
			while (fgets(line,256,file)!=NULL) {
				int len = strlen(line);
				if (len>0 && line[len-1]=='\n') {
					line[len-1] = 0; // trim newline
					len -= 1;
					if (len>0 && line[len-1]=='\r') {
						line[len-1] = 0; // trim Windows newline
						len -= 1;
					}
				}
				if (len==0) continue; // skip empty lines
		
				char disc_path[256];
				strcpy(disc_path, base_path);
				tmp = disc_path + strlen(disc_path);
				strcpy(tmp, line);
				
				// found a valid disc path
				if (exists(disc_path)) {
					disc_paths[total_discs] = strdup(disc_path);
					// matched our current disc
					if (exactMatch(disc_path, rom_path)) {
						rom_disc = total_discs;
						disc = rom_disc;
						sprintf(disc_name, "Disc %i", disc+1);
					}
					total_discs += 1;
				}
			}
			fclose(file);
		}
	}
	
	// m3u path may change rom_file
	sprintf(slot_path, "%s/%s.txt", mmenu_dir, rom_file);
	if (exists(slot_path)) slot = getInt(slot_path);
	if (slot==8) slot = 0;
	
	if (requested_state==REQUEST_SAVE) {
		if (!optional_snapshot) optional_snapshot = SDL_GetVideoSurface();

		char bmp_path[256];
		sprintf(bmp_path, "%s/%s.%d.bmp", mmenu_dir, rom_file, slot);
	
		status = STATUS_SAVESLOT + slot;
		SDL_Surface* preview = createThumbnail(optional_snapshot);
		SDL_RWops* out = SDL_RWFromFile(bmp_path, "wb");
		if (total_discs) {
			char* disc_path = disc_paths[disc];
			putFile(txt_path, disc_path + strlen(base_path));
			sprintf(bmp_path, "%s/%s.%d.bmp", mmenu_dir, rom_file, slot);
		}
		SDL_SaveBMP_RW(preview, out, 1);
		SDL_FreeSurface(preview);
		putInt(slot_path, slot);
	}
	else if (requested_state==REQUEST_LOAD) {
		char save_path[256];
		sprintf(save_path, save_path_template, slot);
		
		if (exists(save_path) && total_discs) {
			char slot_disc_name[256];
			getFile(txt_path, slot_disc_name, 256);
			char slot_disc_path[256];
			if (slot_disc_name[0]=='/') strcpy(slot_disc_path, slot_disc_name);
			else sprintf(slot_disc_path, "%s%s", base_path, slot_disc_name);
			char* disc_path = disc_paths[disc];
			if (!exactMatch(slot_disc_path, disc_path)) {
				putFile(CHANGE_DISC_PATH, slot_disc_path);
			}
		}
		status = STATUS_LOADSLOT + slot;
		putInt(slot_path, slot);
	}
	
	for (int i=0; i<total_discs; i++) {
		free(disc_paths[i]);
	}
	
	return status;
}

MenuReturnStatus ShowMenu(char* rom_path, char* save_path_template, SDL_Surface* optional_snapshot, MenuRequestState requested_state, AutoSave_t autosave) {
	if (requested_state==REQUEST_SAVE || requested_state==REQUEST_LOAD) return SaveLoad(rom_path, save_path_template, optional_snapshot, requested_state, autosave);
	
	screen = SDL_GetVideoSurface();
	GFX_ready();
	SDL_EnableKeyRepeat(300,100);
	
	// path and string things
	char* tmp;
	char rom_file[256]; // with extension
	char rom_name[256]; // without extension or cruft
	char slot_path[256];
	char emu_name[256];
	char mmenu_dir[256];
	
	// filename
	tmp = strrchr(rom_path,'/');
	if (tmp==NULL) tmp = rom_path;
	else tmp += 1;
	strcpy(rom_file, tmp);
	
	getEmuName(rom_path, emu_name);
	sprintf(mmenu_dir, "%s/.mmenu/%s", getenv("USERDATA_PATH"), emu_name);
	mkdir(mmenu_dir, 0755);

	// does this game have an m3u?
	int rom_disc = -1;
	int disc = rom_disc;
	int total_discs = 0;
	char disc_name[16];
	char* disc_paths[9]; // up to 9 paths, Arc the Lad Collection is 7 discs

	// construct m3u path based on parent directory
	// essentially hasM3u() from MiniUI but we use the building blocks as well
	char m3u_path[256];
	strcpy(m3u_path, rom_path);
	tmp = strrchr(m3u_path, '/') + 1;
	tmp[0] = '\0';

	// path to parent directory
	char base_path[256]; // used below too when status==kItemSave
	strcpy(base_path, m3u_path);

	tmp = strrchr(m3u_path, '/');
	tmp[0] = '\0';

	// get parent directory name
	char dir_name[256];
	tmp = strrchr(m3u_path, '/');
	strcpy(dir_name, tmp);

	// dir_name is also our m3u file name
	tmp = m3u_path + strlen(m3u_path); 
	strcpy(tmp, dir_name);

	// add extension
	tmp = m3u_path + strlen(m3u_path);
	strcpy(tmp, ".m3u");
	
	if (exists(m3u_path)) {
		// share saves across multi-disc games
		strcpy(rom_file, dir_name);
		tmp = rom_file + strlen(rom_file);
		strcpy(tmp, ".m3u");
		
		//read m3u file
		FILE* file = fopen(m3u_path, "r");
		if (file) {
			char line[256];
			while (fgets(line,256,file)!=NULL) {
				int len = strlen(line);
				if (len>0 && line[len-1]=='\n') {
					line[len-1] = 0; // trim newline
					len -= 1;
					if (len>0 && line[len-1]=='\r') {
						line[len-1] = 0; // trim Windows newline
						len -= 1;
					}
				}
				if (len==0) continue; // skip empty lines
		
				char disc_path[256];
				strcpy(disc_path, base_path);
				tmp = disc_path + strlen(disc_path);
				strcpy(tmp, line);
				
				// found a valid disc path
				if (exists(disc_path)) {
					disc_paths[total_discs] = strdup(disc_path);
					// matched our current disc
					if (exactMatch(disc_path, rom_path)) {
						rom_disc = total_discs;
						disc = rom_disc;
						sprintf(disc_name, "Disc %i", disc+1);
					}
					total_discs += 1;
				}
			}
			fclose(file);
		}
	}
	
	// m3u path may change rom_file
	sprintf(slot_path, "%s/%s.txt", mmenu_dir, rom_file);
	getDisplayName(rom_file, rom_name);
	
	// cache static elements
	
	// NOTE: original screen copying logic
	// SDL_Surface* copy = SDL_CreateRGBSurface(SDL_SWSURFACE, Screen.width, Screen.height, 16, 0, 0, 0, 0);
	// SDL_BlitSurface(screen, NULL, copy, NULL);

	// NOTE: copying the screen to a new surface caused a 15fps drop in PocketSNES
	// on the 280M running stock OpenDingux after opening the menu, 
	// tried ConvertSurface and DisplaySurface too
	// only this direct copy worked without tanking the framerate
	int copy_bytes = screen->h * screen->pitch;
	void* copy_pixels = malloc(copy_bytes);
	memcpy(copy_pixels, screen->pixels, copy_bytes);
	SDL_Surface* copy = SDL_CreateRGBSurfaceFrom(copy_pixels, screen->w,screen->h, screen->format->BitsPerPixel, screen->pitch, screen->format->Rmask, screen->format->Gmask, screen->format->Bmask, screen->format->Amask);
	
	SDL_Surface* cache = SDL_CreateRGBSurface(SDL_SWSURFACE, SCREEN_WIDTH, SCREEN_HEIGHT, 16, 0, 0, 0, 0);
	SDL_BlitSurface(copy, NULL, cache, NULL);
	SDL_BlitSurface(overlay, NULL, cache, NULL);
	// SDL_FillRect(cache, &(SDL_Rect){0,0,SCREEN_WIDTH,ROW_HEIGHT}, 0);

	GFX_blitText(cache, rom_name, 1, PADDING, 6);
	GFX_blitIngameWindow(cache, 12, 142, 280, 270);
	// SDL_FillRect(cache, &(SDL_Rect){0,SCREEN_HEIGHT-PADDING,SCREEN_WIDTH,PADDING}, 0);
	
	Input_reset();
	
	int status = STATUS_CONTINUE;
	int selected = 0; // resets every launch
	if (exists(slot_path)) slot = getInt(slot_path);
	if (slot==8) slot = 0;
	
	// inline functions? okay.
	void SystemRequest(MenuRequestState request) {
		autosave();
		putFile(AUTO_RESUME_PATH, rom_path + strlen(SDCARD_PATH));
		if (request==REQUEST_SLEEP) {
			fauxSleep();
			unlink(AUTO_RESUME_PATH);
		}
		else powerOff();
	}
	
	if (requested_state!=REQUEST_MENU) { // sleep or poweroff
		if (disable_poweroff && requested_state==REQUEST_POWER) return STATUS_CONTINUE;
		SystemRequest(requested_state);
		if (requested_state==REQUEST_POWER) return REQUEST_POWER;
	}
	
	char save_path[256];
	char bmp_path[256];
	char txt_path[256];
	int save_exists = 0;
	int preview_exists = 0;
	
	int quit = 0;
	int dirty = 1;
	int show_setting = 0; // 1=brightness,2=volume
	int setting_value = 0;
	int setting_min = 0;
	int setting_max = 0;
	unsigned long setting_start = 0;
	unsigned long cancel_start = SDL_GetTicks();
	int was_charging = isCharging();
	unsigned long charge_start = SDL_GetTicks();
	unsigned long power_start = 0;
	while (!quit) {
		unsigned long frame_start = SDL_GetTicks();
		int select_was_pressed = Input_isPressed(BTN_SELECT); // rs90-only
		
		Input_poll();
		
		if (Input_justPressed(BTN_UP)) {
			selected -= 1;
			if (selected<0) selected += MAX_ITEMS;
			dirty = 1;
		}
		else if (Input_justPressed(BTN_DOWN)) {
			selected += 1;
			if (selected>=MAX_ITEMS) selected -= MAX_ITEMS;
			dirty = 1;
		}
		else if (Input_justPressed(BTN_LEFT)) {
			if (total_discs>1 && selected==ITEM_CONTINUE) {
				disc -= 1;
				if (disc<0) disc += total_discs;
				dirty = 1;
				sprintf(disc_name, "Disc %i", disc+1);
			}
			else if (selected==ITEM_SAVE || selected==ITEM_LOAD) {
				slot -= 1;
				if (slot<0) slot += MAX_SLOTS;
				dirty = 1;
			}
		}
		else if (Input_justPressed(BTN_RIGHT)) {
			if (total_discs>1 && selected==ITEM_CONTINUE) {
				disc += 1;
				if (disc==total_discs) disc -= total_discs;
				dirty = 1;
				sprintf(disc_name, "Disc %i", disc+1);
			}
			else if (selected==ITEM_SAVE || selected==ITEM_LOAD) {
				slot += 1;
				if (slot>=MAX_SLOTS) slot -= MAX_SLOTS;
				dirty = 1;
			}
		}
		
		if (dirty && state_support && (selected==ITEM_SAVE || selected==ITEM_LOAD)) {
			sprintf(save_path, save_path_template, slot);
			sprintf(bmp_path, "%s/%s.%d.bmp", mmenu_dir, rom_file, slot);
			sprintf(txt_path, "%s/%s.%d.txt", mmenu_dir, rom_file, slot);
		
			save_exists = exists(save_path);
			preview_exists = save_exists && exists(bmp_path);
			// printf("save_path: %s (%i)\n", save_path, save_exists);
			// printf("bmp_path: %s (%i)\n", bmp_path, preview_exists);
		}
		
		if ((Input_justPressed(BTN_B) || Input_justReleased(BTN_MENU)) && !dirty && show_setting == 0) {
			status = STATUS_CONTINUE;
			quit = 1;
		}
		else if (Input_justPressed(BTN_A)) {
			switch(selected) {
				case ITEM_CONTINUE:
				if (total_discs && rom_disc!=disc) {
						status = STATUS_CHANGEDISC;
						char* disc_path = disc_paths[disc];
						putFile(CHANGE_DISC_PATH, disc_path);
					}
					else {
						status = STATUS_CONTINUE;
					}
					quit = 1;
				break;
				
				// TODO: this code is duplicated in SaveLoad()
				case ITEM_SAVE:
				if (state_support) {
					status = STATUS_SAVESLOT + slot;
					SDL_Surface* preview = createThumbnail(optional_snapshot ? optional_snapshot : copy);
					SDL_RWops* out = SDL_RWFromFile(bmp_path, "wb");
					if (total_discs) {
						char* disc_path = disc_paths[disc];
						putFile(txt_path, disc_path + strlen(base_path));
						sprintf(bmp_path, "%s/%s.%d.bmp", mmenu_dir, rom_file, slot);
					}
					SDL_SaveBMP_RW(preview, out, 1);
					SDL_FreeSurface(preview);
					putInt(slot_path, slot);
					quit = 1;
				}
				break;
				case ITEM_LOAD:
				if (state_support) {
					if (save_exists && total_discs) {
						char slot_disc_name[256];
						getFile(txt_path, slot_disc_name, 256);
						char slot_disc_path[256];
						if (slot_disc_name[0]=='/') strcpy(slot_disc_path, slot_disc_name);
						else sprintf(slot_disc_path, "%s%s", base_path, slot_disc_name);
						char* disc_path = disc_paths[disc];
						if (!exactMatch(slot_disc_path, disc_path)) {
							putFile(CHANGE_DISC_PATH, slot_disc_path);
						}
					}
					status = STATUS_LOADSLOT + slot;
					putInt(slot_path, slot);
					quit = 1;
				}
				break;
				case ITEM_OPTIONS:
					status = is_simple ? STATUS_RESET : STATUS_OPENMENU;
					quit = 1;
				break;
				case ITEM_QUIT:
					status = STATUS_EXIT;
					quit = 1;
				break;
			}
			if (quit) break;
		}
		
		unsigned long now = SDL_GetTicks();
		if (Input_anyPressed()) cancel_start = now;

		#define CHARGE_DELAY 1000
		if (dirty || now-charge_start>=CHARGE_DELAY) {
			int is_charging = isCharging();
			if (was_charging!=is_charging) {
				was_charging = is_charging;
				dirty = 1;
			}
			charge_start = now;
		}

		if (!disable_poweroff && power_start && now-power_start>=1000) {
			SystemRequest(REQUEST_POWER);
			status = STATUS_POWER;
			quit = 1;
		}
		if (Input_justPressed(BTN_POWER)) {
			power_start = now;
		}
		
		#define SLEEP_DELAY 30000
		if (now-cancel_start>=SLEEP_DELAY && preventAutosleep()) cancel_start = now;
		
		if (now-cancel_start>=SLEEP_DELAY || Input_justReleased(BTN_POWER)) // || Input_justPressed(kButtonMenu)) 
		{
			SystemRequest(REQUEST_SLEEP);
			cancel_start = SDL_GetTicks();
			power_start = 0;
			dirty = 1;
		}
		
		int old_setting = show_setting;
		int old_value = setting_value;
		show_setting = 0;
		if (Input_isPressed(BTN_START) && Input_isPressed(BTN_SELECT)) {
			// buh
		}
		else if (Input_isPressed(BTN_START) && (Input_isPressed(BTN_MINUS) || Input_isPressed(BTN_PLUS))) {
			show_setting = 1;
			setting_value = GetBrightness();
			setting_min = MIN_BRIGHTNESS;
			setting_max = MAX_BRIGHTNESS;
		}
		else if (Input_isPressed(BTN_START) && old_setting == 1) {
			show_setting = 1;
			setting_value = GetBrightness();
			setting_min = MIN_BRIGHTNESS;
			setting_max = MAX_BRIGHTNESS;
		}
		else if (Input_isPressed(BTN_MINUS) || Input_isPressed(BTN_PLUS)) {
			show_setting = 2;
			setting_value = GetVolume();
			setting_min = MIN_VOLUME;
			setting_max = MAX_VOLUME;
		}

		if (old_setting && !show_setting) setting_start = SDL_GetTicks();

		if (old_value != setting_value) dirty = 1;
		else if (!old_setting && show_setting) dirty = 1;
		else if (setting_start > 0 && SDL_GetTicks() - setting_start > 500) {
			dirty = 1;
			setting_start = 0;
		}
		
		if (dirty) {
			dirty = 0;
			SDL_BlitSurface(cache, NULL, screen, NULL);
			GFX_blitBattery(screen, 576, 12);
			
			if (show_setting) {
				GFX_blitSettings(screen, 0, 0, show_setting==VOLUME_ICON?BRIGHTNESS_ICON:(setting_value>BRIGHTNESS_ICON?VOLUME_ICON:VOLUME_MUTE_ICON), setting_value,setting_min,setting_max);
			}
			
			// list
			SDL_Surface* text;
			for (int i=0; i<MAX_ITEMS; i++) {
				char* item = items[i];
				int disabled = !state_support && (i==ITEM_SAVE || i==ITEM_LOAD);
				int color = disabled ? -1 : 1; // gray or gold
				if (i==selected) {
					SDL_FillRect(screen, &(SDL_Rect){12,152+(i*44)-((44)/2),280,44}, SDL_MapRGB(screen->format, TRIAD_WHITE));
				}
				// if (i==selected) {
				// 	int bg_color_rgb = disabled ? gray_rgb : pink_rgb;
				// 	SDL_FillRect(screen, &(SDL_Rect){Screen.menu.window.x,Screen.menu.list.y+(i*Screen.menu.list.line_height)-((Screen.menu.list.row_height-Screen.menu.list.line_height)/2),Screen.menu.window.width,Screen.menu.list.row_height}, bg_color_rgb);
				// 	if (!disabled) color = 0; // white
				// }
				
				GFX_blitText(screen, item, 2, PADDING, 152+(i*44));

				if ((state_support && (i==ITEM_SAVE || i==ITEM_LOAD)) || (total_discs>1 && i==ITEM_CONTINUE)) {
					SDL_BlitSurface(i==selected?arrow_highlighted:arrow, NULL, screen, &(SDL_Rect){12+280-(arrow->w+12),152+(i*44)+14});
				}
			}
			
			// disc change
			if (total_discs>1 && selected==ITEM_CONTINUE) {
				GFX_blitIngameWindow(screen, 296, 142, 332, 44+(8*2));
				GFX_blitText(screen, disc_name, 2, 296+130, 152);
			}
			// slot preview
			else if (state_support && (selected==ITEM_SAVE || selected==ITEM_LOAD)) {
				// preview window
				SDL_Rect preview_rect = {296+6,142+6};
				GFX_blitIngameWindow(screen, 296, 142, 332, 270);
				
				if (preview_exists) { // has save, has preview
					SDL_Surface* preview = IMG_Load(bmp_path);
					if (!preview) printf("IMG_Load: %s\n", IMG_GetError());
					SDL_BlitSurface(preview, NULL, screen, &preview_rect);
					SDL_FreeSurface(preview);
				}
				else {
					int hw = SCREEN_WIDTH / 2;
					int hh = SCREEN_HEIGHT / 2;
					SDL_FillRect(screen, &(SDL_Rect){296+6,142+6,hw,hh}, 0);
					if (save_exists) { // has save but no preview
						SDL_BlitSurface(no_preview, NULL, screen, &(SDL_Rect){
							296+6+(hw-no_preview->w)/2,
							142+6+(hh-no_preview->h)/2
						});
					}
					else { // no save
						SDL_BlitSurface(empty_slot, NULL, screen, &(SDL_Rect){
							296+6+(hw-empty_slot->w)/2,
							142+6+(hh-empty_slot->h)/2
						});
					}
				}
				
				SDL_BlitSurface(slot_overlay, NULL, screen, &preview_rect);
				SDL_BlitSurface(slot_dots, NULL, screen, &(SDL_Rect){400,394});
				SDL_BlitSurface(slot_dot_selected, NULL, screen, &(SDL_Rect){400+(16*slot),394});
			}


			int btn_a_width = GFX_getButtonWidth("Open", "A");
			GFX_blitButton(screen, "A", "Okay", 557, 419);
			GFX_blitButton(screen, "B", "Back", 557-btn_a_width,419);
			SDL_Flip(screen);
		}
		
		// slow down to 60fps
		unsigned long frame_duration = SDL_GetTicks() - frame_start;
		#define FRAME_DRATION 17
		if (frame_duration<FRAME_DRATION) SDL_Delay(FRAME_DRATION-frame_duration);
	}
	
	// redraw original screen before returning
	if (status!=STATUS_POWER) {
		SDL_FillRect(screen, NULL, 0);
		SDL_BlitSurface(copy, NULL, screen, NULL);
		SDL_Flip(screen);
	}

	SDL_FreeSurface(cache);
	// NOTE: copy->pixels was manually malloc'd so it must be manually freed too
	SDL_FreeSurface(copy);
	free(copy_pixels); 
	
	SDL_EnableKeyRepeat(0,0);
	
	// if (SDL_MUSTLOCK(screen)) SDL_LockSurface(screen); // fix for regba?
	for (int i=0; i<total_discs; i++) {
		free(disc_paths[i]);
	}
	
	return status;
}

int ResumeSlot(void) {
	if (!exists(RESUME_SLOT_PATH)) return -1;
	
	slot = getInt(RESUME_SLOT_PATH);
	unlink(RESUME_SLOT_PATH);

	return slot;
}

int ChangeDisc(char* disc_path) {
	if (!exists(CHANGE_DISC_PATH)) return 0;
	getFile(CHANGE_DISC_PATH, disc_path, 256);
	return 1;
}