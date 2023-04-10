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

///////////////////////////////////////////////////////////////
// Video screen surface
static SDL_Surface* screen;
// UI Elements
static struct GFX_Context {
	SDL_Surface* overlay;
	SDL_Surface* slot_overlay;
	SDL_Surface* slot_pagination;
	SDL_Surface* slot_active;
	SDL_Surface* Slot_preview;
	SDL_Surface* no_preview;
	SDL_Surface* empty_slot;
} gfx;

static char* menu_items[MAX_ITEMS];
static int game_slot = 0;
static int state_support = 1;
static int disable_poweroff = 0;
int show_menu = 0;
Rom game;
/////////////////////////////////////////////////////////////
// Run these functions first and after main
void menu_init() __attribute__((constructor));
void menu_quit() __attribute__((destructor));

void menu_init(void) {
	void* librt = dlopen("librt.so.1", RTLD_LAZY | RTLD_GLOBAL); // shm
	void* libmsettings = dlopen("libmsettings.so", RTLD_LAZY | RTLD_GLOBAL);
	InitSettings();

	menu.simple_mode = exists(SIMPLE_MODE_PATH);

	menu_items[ITEM_CONTINUE] = "Continue";
	menu_items[ITEM_SAVE] = "Save";
	menu_items[ITEM_LOAD] = "Load";
	menu_items[ITEM_OPTIONS] = menu.simple_mode ? "Reset" : "Options";
	menu_items[ITEM_QUIT] = "Quit";

	GFX_init();
	assets_init();
}

void assets_init(void) {
	// Creates a darken overlay on screen for ingame menu
	gfx.overlay = SDL_CreateRGBSurface(SDL_SWSURFACE, SCREEN_WIDTH, SCREEN_HEIGHT, 16, 0, 0, 0, 0);
	gfx.slot_overlay = GFX_loadImage("slot-overlay.png");
	gfx.slot_pagination = GFX_loadImage("slot-dots.png");
	gfx.slot_active = GFX_loadImage("slot-dot-selected.png");
	gfx.no_preview = GFX_getText("No Preview");
	gfx.empty_slot = GFX_getText("Empty Slot");

	SDL_SetAlpha(gfx.overlay, SDL_SRCALPHA, 0x80);
	SDL_FillRect(gfx.overlay, NULL, 0);
}

void menu_quit(void) {
	SDL_FreeSurface(gfx.overlay);
	SDL_FreeSurface(gfx.slot_overlay);
	SDL_FreeSurface(gfx.slot_pagination);
	SDL_FreeSurface(gfx.slot_active);
	SDL_FreeSurface(gfx.no_preview);
	SDL_FreeSurface(gfx.empty_slot);
}
/////////////////////////////////////////////////////////////
// Utilities functions for ingame menu
// Create thumbnails
SDL_Surface* slot_thumbnail(SDL_Surface* src_img) {
	SDL_Surface* dst_img;
	dst_img = SDL_CreateRGBSurface(0,SCREEN_WIDTH/2, SCREEN_HEIGHT/2,src_img->format->BitsPerPixel,src_img->format->Rmask,src_img->format->Gmask,src_img->format->Bmask,src_img->format->Amask);

	uint8_t* src_px = src_img->pixels;
	uint8_t* dst_px = dst_img->pixels;
	int step = dst_img->format->BytesPerPixel;
	int step_double = step * 2;
	int stride = src_img->pitch;
	for (int i=0; i<dst_img->h; i++) {
		for (int j=0; j<dst_img->w; j++) {
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
			src_px += step_double;
		}
		src_px += stride;
	}

	return dst_img;
}

void create_thumbnail(Rom game, int status, SDL_Surface* optional_snapshot) {
	status = STATUS_SAVESLOT + game_slot;
	SDL_Surface* preview = slot_thumbnail(optional_snapshot);
	SDL_RWops* out = SDL_RWFromFile(game.bmp_path, "wb");
	if (game.total_discs) {
		char* disc_path = disc_paths[game.disc];
		putFile(game.txt_path, disc_path + strlen(game.base_path));
		sprintf(game.bmp_path, "%s/%s.%d.bmp", game.mmenu_dir, game.rom_file, game_slot);
	}
	SDL_SaveBMP_RW(preview, out, 1);
	SDL_FreeSurface(preview);
	putInt(game.slot_path, game_slot);
}

// Warning for unsupported state, TODO: style as toast message
void show_warning(void) {
	state_support = 0;
	disable_poweroff = 1;
	disablePoweroff();
	
	screen = SDL_GetVideoSurface();
	GFX_ready();

	GFX_blitParagraph(screen, "This pak does not support\nsave states, auto-power off\nor quicksave and resume.", 0,0,SCREEN_WIDTH/2,SCREEN_HEIGHT/2);
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
}


// TODO: make this a reusable function, is used throughout this file
void load_game(char* rom_path, Rom game, int show_menu = 0) {
	char* tmp;
	tmp = strrchr(rom_path,'/');
	if (tmp==NULL) tmp = rom_path;
	else tmp += 1;
	strcpy(game.rom_file, tmp);

	getEmuName(rom_path, game.emu_name);
	sprintf(game.mmenu_dir, "%s/.mmenu/%s", getenv("USERDATA_PATH"), game.emu_name);
	mkdir(game.mmenu_dir, 0755);

	if (!show_menu) sprintf(game.txt_path, "%s/%s.%d.txt", game.mmenu_dir, game.rom_file, game_slot);

	game.disc = game.rom_disc;
	game.total_discs = 0;
	char* disc_paths[9]; // up to 9 paths, Arc the Lad Collection is 7 discs

	strcpy(game.m3u_path, rom_path);
	tmp = strrchr(game.m3u_path, '/') + 1;
	tmp[0] = '\0';

	strcpy(game.base_path, game.m3u_path);

	tmp = strrchr(game.m3u_path, '/');
	tmp[0] = '\0';

	tmp = strrchr(game.m3u_path, '/');
	strcpy(game.dir_name, tmp);

	tmp = game.m3u_path + strlen(game.m3u_path); 
	strcpy(tmp, game.dir_name);

	tmp = game.m3u_path + strlen(game.m3u_path);
	strcpy(tmp, ".m3u");

	if (exists(game.m3u_path)) {
		// share saves across multi-disc games
		strcpy(game.rom_file, game.dir_name);
		tmp = game.rom_file + strlen(game.rom_file);
		strcpy(tmp, ".m3u");
		
		//read m3u file
		FILE* file = fopen(game.m3u_path, "r");
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
				strcpy(disc_path, game.base_path);
				tmp = disc_path + strlen(disc_path);
				strcpy(tmp, line);
				
				// found a valid disc path
				if (exists(disc_path)) {
					disc_paths[game.total_discs] = strdup(disc_path);
					// matched our current disc
					if (exactMatch(disc_path, rom_path)) {
						game.rom_disc = game.total_discs;
						game.disc = game.rom_disc;
						sprintf(game.disc_name, "Disc %i", game.disc+1);
					}
					game.total_discs += 1;
				}
			}
			fclose(file);
		}
	}
}

// Menu input events
void input_events(Rom game, int selected, int status, SDL_Surface* optional_snapshot) {
		int select_was_pressed = Input_isPressed(BTN_SELECT); // rs90-only
		Input_poll();

		if (Input_justPressed(BTN_UP)) {
			selected -= 1;
			if (selected<0) selected += MAX_ITEMS;
			menu.dirty = 1;
		}
		else if (Input_justPressed(BTN_DOWN)) {
			selected += 1;
			if (selected>=MAX_ITEMS) selected -= MAX_ITEMS;
			menu.dirty = 1;
		}
		else if (Input_justPressed(BTN_LEFT)) {
			if (game.total_discs>1 && selected==ITEM_CONTINUE) {
				game.disc -= 1;
				if (game.disc<0) game.disc += game.total_discs;
				menu.dirty = 1;
				sprintf(game.disc_name, "Disc %i", game.disc+1);
			}
			else if (selected==ITEM_SAVE || selected==ITEM_LOAD) {
				game_slot -= 1;
				if (game_slot<0) game_slot += MAX_SLOTS;
				menu.dirty = 1;
			}
		}
		else if (Input_justPressed(BTN_RIGHT)) {
			if (game.total_discs>1 && selected==ITEM_CONTINUE) {
				game.disc += 1;
				if (game.disc==game.total_discs) game.disc -= game.total_discs;
				menu.dirty = 1;
				sprintf(game.disc_name, "Disc %i", game.disc+1);
			}
			else if (selected==ITEM_SAVE || selected==ITEM_LOAD) {
				game_slot += 1;
				if (game_slot>=MAX_SLOTS) game_slot -= MAX_SLOTS;
				menu.dirty = 1;
			}
		}

		//NOTE: may have to include the dirty and state checks above here
		if ((Input_justPressed(BTN_B) || Input_justReleased(BTN_MENU)) && !menu.dirty && menu.show_setting == 0) {
			status = STATUS_CONTINUE;
			menu.quit = 1;
		}
		else if (Input_justPressed(BTN_A)) {
			switch(selected) {
				case ITEM_CONTINUE:
				if (game.total_discs && game.rom_disc!=game.disc) {
						status = STATUS_CHANGEDISC;
						char* disc_path = disc_paths[game.disc];
						putFile(CHANGE_DISC_PATH, disc_path);
					}
					else {
						status = STATUS_CONTINUE;
					}
					menu.quit = 1;
				break;
				case ITEM_SAVE:
				if (state_support) {
					create_thumbnail(game, status, optional_snapshot);
					menu.quit = 1;
				}
				break;
				case ITEM_LOAD:
				if (state_support) {
					if (menu.save_exists && game.total_discs) {
						char slot_disc_name[256];
						getFile(game.txt_path, slot_disc_name, 256);
						char slot_disc_path[256];
						if (slot_disc_name[0]=='/') strcpy(slot_disc_path, slot_disc_name);
						else sprintf(slot_disc_path, "%s%s", game.base_path, slot_disc_name);
						char* disc_path = disc_paths[game.disc];
						if (!exactMatch(slot_disc_path, disc_path)) {
							putFile(CHANGE_DISC_PATH, slot_disc_path);
						}
					}
					status = STATUS_LOADSLOT + game_slot;
					putInt(game.slot_path, game_slot);
					menu.quit = 1;
				}
				break;
				case ITEM_OPTIONS:
					status = menu.simple_mode ? STATUS_RESET : STATUS_OPENMENU;
					menu.quit = 1;
				break;
				case ITEM_QUIT:
					status = STATUS_EXIT;
					menu.quit = 1;
				break;
			}
			if (menu.quit) break;
		}
}

// Save and load game
int SaveLoad(char* rom_path, char* save_path_template, SDL_Surface* optional_snapshot, int requested_state, AutoSave_t autosave) {
	int status = STATUS_CONTINUE;
	if (!state_support) return status;
	load_game(rom_path, game);

	sprintf(game.slot_path, "%s/%s.txt", game.mmenu_dir, game.rom_file);
	if (exists(game.slot_path)) game_slot = getInt(game.slot_path);
	if (game_slot==8) game_slot = 0;
	
	if (requested_state==REQUEST_SAVE) {
		if (!optional_snapshot) optional_snapshot = SDL_GetVideoSurface();
		sprintf(game.bmp_path, "%s/%s.%d.bmp", game.mmenu_dir, game.rom_file, game_slot);
		create_thumbnail(game, status, optional_snapshot);
	}
	else if (requested_state==REQUEST_LOAD) {
		char save_path[256];
		sprintf(save_path, save_path_template, game_slot);
		
		if (exists(save_path) && game.total_discs) {
			char slot_disc_name[256];
			getFile(game.txt_path, slot_disc_name, 256);
			char slot_disc_path[256];
			if (slot_disc_name[0]=='/') strcpy(slot_disc_path, slot_disc_name);
			else sprintf(slot_disc_path, "%s%s", game.base_path, slot_disc_name);
			char* disc_path = disc_paths[game.disc];
			if (!exactMatch(slot_disc_path, disc_path)) {
				putFile(CHANGE_DISC_PATH, slot_disc_path);
			}
		}
		status = STATUS_LOADSLOT + game_slot;
		putInt(game.slot_path, game_slot);
	}
	
	for (int i=0; i<game.total_discs; i++) {
		free(disc_paths[i]);
	}
	return status;
}

// Open menu
int ShowMenu(char* rom_path, char* save_path_template, SDL_Surface* optional_snapshot, int requested_state, AutoSave_t autosave) {
	show_menu = 1;
	if (requested_state==REQUEST_SAVE || requested_state==REQUEST_LOAD) return SaveLoad(rom_path, save_path_template, optional_snapshot, requested_state, autosave);
	screen = SDL_GetVideoSurface();
	GFX_ready();
	SDL_EnableKeyRepeat(300,100);

	load_game(rom_path, game, show_menu);
	sprintf(game.slot_path, "%s/%s.txt", game.mmenu_dir, game.rom_file);
	getDisplayName(game.rom_file, game.rom_name);

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
	SDL_BlitSurface(gfx.overlay, NULL, cache, NULL);
	// SDL_FillRect(cache, &(SDL_Rect){0,0,SCREEN_WIDTH,ROW_HEIGHT}, 0);

	GFX_blitText(cache, game.rom_name, 1, PADDING, 6);
	GFX_blitIngameWindow(cache, 12, 142, 280, 270);

	Input_reset();

	int status = STATUS_CONTINUE;
	int selected = 0; // resets every launch
	if (exists(game.slot_path)) game_slot = getInt(game.slot_path);
	if (game_slot==8) game_slot = 0;

	// sleep or poweroff
	if (requested_state!=REQUEST_MENU) {
		if (disable_poweroff && requested_state==REQUEST_POWER) return STATUS_CONTINUE;
		SystemRequest(requested_state);
		if (requested_state==REQUEST_POWER) return REQUEST_POWER;
	}

	char save_path[256];
	char bmp_path[256];
	char txt_path[256];

	unsigned long setting_start = 0;
	unsigned long cancel_start = SDL_GetTicks();
	int was_charging = isCharging();
	unsigned long charge_start = SDL_GetTicks();
	unsigned long power_start = 0;

	while (!menu.quit) {
		unsigned long frame_start = SDL_GetTicks();
		input_events(game, selected, status, optional_snapshot);

		if (menu.dirty && state_support && (selected==ITEM_SAVE || selected==ITEM_LOAD)) {
			sprintf(save_path, save_path_template, game_slot);
			sprintf(bmp_path, "%s/%s.%d.bmp", game.mmenu_dir, game.rom_file, game_slot);
			sprintf(txt_path, "%s/%s.%d.txt", game.mmenu_dir, game.rom_file, game_slot);
		
			menu.save_exists = exists(save_path);
			menu.preview_exists = menu.save_exists && exists(bmp_path);
			// printf("save_path: %s (%i)\n", save_path, save_exists);
			// printf("bmp_path: %s (%i)\n", bmp_path, preview_exists);
		}

		unsigned long now = SDL_GetTicks();
		if (Input_anyPressed()) cancel_start = now;

		if (menu.dirty || now-charge_start>=CHARGE_DELAY) {
			int is_charging = isCharging();
			if (was_charging!=is_charging) {
				was_charging = is_charging;
				menu.dirty = 1;
			}
			charge_start = now;
		}
		if (!disable_poweroff && power_start && now-power_start>=1000) {
			SystemRequest(REQUEST_POWER);
			status = STATUS_POWER;
			menu.quit = 1;
		}
		if (Input_justPressed(BTN_POWER)) {
			power_start = now;
		}
		
		if (now-cancel_start>=SLEEP_DELAY && preventAutosleep()) cancel_start = now;
		
		if (now-cancel_start>=SLEEP_DELAY || Input_justReleased(BTN_POWER)) // || Input_justPressed(kButtonMenu)) 
		{
			SystemRequest(REQUEST_SLEEP);
			cancel_start = SDL_GetTicks();
			power_start = 0;
			menu.dirty = 1;
		}
		
		int old_setting = menu.show_setting;
		int old_value = menu.setting_value;
		menu.show_setting = 0;

		if (Input_isPressed(BTN_START) && (Input_isPressed(BTN_MINUS) || Input_isPressed(BTN_PLUS))) {
			menu.show_setting = 1;
			menu.setting_value = GetBrightness();
			menu.setting_min = MIN_BRIGHTNESS;
			menu.setting_max = MAX_BRIGHTNESS;
		}
		else if (Input_isPressed(BTN_START) && old_setting == 1) {
			menu.show_setting = 1;
			menu.setting_value = GetBrightness();
			menu.setting_min = MIN_BRIGHTNESS;
			menu.setting_max = MAX_BRIGHTNESS;
		}
		else if (Input_isPressed(BTN_MINUS) || Input_isPressed(BTN_PLUS)) {
			menu.show_setting = 2;
			menu.setting_value = GetVolume();
			menu.setting_min = MIN_VOLUME;
			menu.setting_max = MAX_VOLUME;
		}

		if (old_setting && !menu.show_setting) setting_start = SDL_GetTicks();

		if (old_value != menu.setting_value) menu.dirty = 1;
		else if (!old_setting && menu.show_setting) menu.dirty = 1;
		else if (setting_start > 0 && SDL_GetTicks() - setting_start > 500) {
			menu.dirty = 1;
			setting_start = 0;
		}
		/// MENU UI display
		if (menu.dirty) {
			gfx_menu(cache,selected);
		}

		// slow down to 60fps
		unsigned long frame_duration = SDL_GetTicks() - frame_start;
		if (frame_duration<FRAME_DELAY) SDL_Delay(FRAME_DELAY-frame_duration);
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
	for (int i=0; i<game.total_discs; i++) {
		free(disc_paths[i]);
	}

	return status;
}

void SystemRequest(in request) {
	autosave();
	putFile(AUTO_RESUME_PATH, game.rom_path + strlen(SDCARD_PATH));
	if (request==REQUEST_SLEEP) {
		fauxSleep();
		unlink(AUTO_RESUME_PATH);
	}
	else powerOff();
}

int ResumeSlot(void) {
	if (!exists(RESUME_SLOT_PATH)) return -1;
	
	game_slot = getInt(RESUME_SLOT_PATH);
	unlink(RESUME_SLOT_PATH);

	return game_slot;
}

int ChangeDisc(char* disc_path) {
	if (!exists(CHANGE_DISC_PATH)) return 0;
	getFile(CHANGE_DISC_PATH, disc_path, 256);
	return 1;
}
/////////////////////////////////////////////////////////////
// Menu UI
void gfx_menu(SDL_Surface* cache, int selected) {
	menu.dirty = 0;
	SDL_BlitSurface(cache, NULL, screen, NULL);
	GFX_blitBattery(screen, 576, 12);
	// Brightness / Volumne controls
	if (menu.show_setting) {
		GFX_blitSettings(screen, 0, 0, menu.show_setting==VOLUME_ICON?BRIGHTNESS_ICON:(setting_value>BRIGHTNESS_ICON?VOLUME_ICON:VOLUME_MUTE_ICON), setting_value,setting_min,setting_max);
	}
	
	// Settings list
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
		if ((state_support && (i==ITEM_SAVE || i==ITEM_LOAD)) || (game.total_discs>1 && i==ITEM_CONTINUE)) {
			// SDL_BlitSurface(i==selected?arrow_highlighted:arrow, NULL, screen, &(SDL_Rect){12+280-(arrow->w+12),152+(i*44)+14});
		}
	}

	// disc change
	if (game.total_discs>1 && selected==ITEM_CONTINUE) {
		GFX_blitIngameWindow(screen, 296, 142, 332, 44+(8*2));
		GFX_blitText(screen, disc_name, 2, 296+130, 152);
	}
	// slot preview
	else if (state_support && (selected==ITEM_SAVE || selected==ITEM_LOAD)) {
		// preview window
		SDL_Rect preview_rect = {296+6,142+6};
		GFX_blitIngameWindow(screen, 296, 142, 332, 270);
		
		if (menu.preview_exists) { // has save, has preview
			SDL_Surface* preview = IMG_Load(game.bmp_path);
			if (!preview) printf("IMG_Load: %s\n", IMG_GetError());
			SDL_BlitSurface(preview, NULL, screen, &preview_rect);
			SDL_FreeSurface(preview);
		}
		else {
			int hw = SCREEN_WIDTH / 2;
			int hh = SCREEN_HEIGHT / 2;
			SDL_FillRect(screen, &(SDL_Rect){296+6,142+6,hw,hh}, 0);
			if (menu.save_exists) { // has save but no preview
				SDL_BlitSurface(gfx.no_preview, NULL, screen, &(SDL_Rect){
					296+6+(hw-no_preview->w)/2,
					142+6+(hh-no_preview->h)/2
				});
			}
			else { // no save
				SDL_BlitSurface(gfx.empty_slot, NULL, screen, &(SDL_Rect){
					296+6+(hw-empty_slot->w)/2,
					142+6+(hh-empty_slot->h)/2
				});
			}
		}
		
		SDL_BlitSurface(gfx.slot_overlay, NULL, screen, &preview_rect);
		SDL_BlitSurface(gfx.slot_pagination, NULL, screen, &(SDL_Rect){400,394});
		SDL_BlitSurface(gfx.slot_active, NULL, screen, &(SDL_Rect){400+(16*slot),394});
	}

	int btn_a_width = GFX_getButtonWidth("Open", "A");
	GFX_blitButton(screen, "A", "Okay", 557, 419);
	GFX_blitButton(screen, "B", "Back", 557-btn_a_width,419);
	SDL_Flip(screen);
}