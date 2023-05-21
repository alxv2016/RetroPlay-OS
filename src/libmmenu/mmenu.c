#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <sys/stat.h>
#include <unistd.h>
#include <msettings.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

#include "../common/defines.h"
#include "../common/keycontext.h"

#include "../common/utils.h"
#include "../common/api.h"
#include "../common/controls.h"
#include "../common/interface.h"

#include "mmenu.h"

static int slot = 0;

__attribute__((constructor)) static void MMenu_init(void) {
	void* librt = dlopen("librt.so.1", RTLD_LAZY | RTLD_GLOBAL);
	void* libmsettings = dlopen("libmsettings.so", RTLD_LAZY | RTLD_GLOBAL);
	InitSettings();

	gfx.overlay = SDL_CreateRGBSurface(SDL_SWSURFACE, SCREEN_WIDTH, SCREEN_HEIGHT, 16, 0, 0, 0, 0);
	SDL_SetAlpha(gfx.overlay, SDL_SRCALPHA, 0x80);
	SDL_FillRect(gfx.overlay, NULL, 0);
}
__attribute__((destructor)) static void MMenu_quit(void) {
	SDL_FreeSurface(gfx.overlay);
}

static MenuReturnStatus SaveLoad(char* rom_path, char* save_path_template, SDL_Surface* optional_snapshot, MenuRequestState requested_state, AutoSave_t autosave) {
	int status = kStatusContinue;
	return status;
}

MenuReturnStatus ShowMenu(char* rom_path, char* save_path_template, SDL_Surface* optional_snapshot, MenuRequestState requested_state, AutoSave_t autosave) {

	// gfx.screen = SDL_GetVideoSurface();
	// SDL_BlitSurface(gfx.overlay, NULL, gfx.screen, NULL);
	SDL_EnableKeyRepeat(300,100);
	Input_reset();

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

	char save_path[256];
	char bmp_path[256];
	char txt_path[256];
	int save_exists = 0;
	int preview_exists = 0;

	if (exists(slot_path)) slot = getInt(slot_path);
	if (slot==8) slot = 0;

	int status = kStatusContinue;
	int quit = 0;
	int dirty = 1;

	
	while (!quit) {
		unsigned long frameStart = SDL_GetTicks();
		Input_poll();

		if ((Input_justPressed(BTN_B) || Input_justReleased(BTN_MENU)) && !dirty) {
			status = kStatusOpenMenu;
			quit = 1;
		}
		else if (Input_justPressed(BTN_A)) {
			status = kStatusExitGame;
			quit = 1;
			if (quit) break;
		}
		
		if (dirty) {
			dirty = 0;
			// SDL_Flip(gfx.screen);
		}
		
		// slow down to 60fps
		GFX_sync(frameStart);
	}
	
	// redraw original screen before returning
	if (status!=kStatusPowerOff) {
		// SDL_FillRect(gfx.screen, NULL, 0);
		// SDL_Flip(gfx.screen);
	}

	SDL_EnableKeyRepeat(0,0);
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