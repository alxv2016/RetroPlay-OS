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
#include "../common/controls.h"
#include "../common/interface.h"

#include "mmenu.h"

static SDL_Surface* overlay;
static SDL_Surface* screen;
static int slot = 0;

__attribute__((constructor)) static void init(void) {
	void* librt = dlopen("librt.so.1", RTLD_LAZY | RTLD_GLOBAL); // shm
	void* libmsettings = dlopen("libmsettings.so", RTLD_LAZY | RTLD_GLOBAL);
	InitSettings();

	overlay = SDL_CreateRGBSurface(SDL_SWSURFACE, SCREEN_WIDTH, SCREEN_HEIGHT, 16, 0, 0, 0, 0);
	SDL_SetAlpha(overlay, SDL_SRCALPHA, 0x80);
	SDL_FillRect(overlay, NULL, 0);
}
__attribute__((destructor)) static void quit(void) {
	SDL_FreeSurface(overlay);
}

MenuReturnStatus ShowMenu(char* rom_path, char* save_path_template, SDL_Surface* optional_snapshot, MenuRequestState requested_state, AutoSave_t autosave) {
	// if (requested_state==kRequestSave || requested_state==kRequestLoad) return SaveLoad(rom_path, save_path_template, optional_snapshot, requested_state, autosave);
	
	screen = SDL_GetVideoSurface();
	SDL_EnableKeyRepeat(300,100);
	SDL_BlitSurface(overlay, NULL, screen, NULL);
	Input_reset();

	int status = kStatusContinue;
	int quit = 0;
	int dirty = 1;
	
	while (!quit) {
		unsigned long frame_start = SDL_GetTicks();
		Input_poll();

		if ((Input_justPressed(BTN_B) || Input_justReleased(BTN_MENU)) && !dirty) {
			status = kStatusContinue;
			quit = 1;
		}
		else if (Input_justPressed(BTN_A)) {
			status = kStatusContinue;
			quit = 1;
			if (quit) break;
		}
		
		if (dirty) {
			dirty = 0;
			SDL_Flip(screen);
		}
		
		// slow down to 60fps
		unsigned long frame_duration = SDL_GetTicks() - frame_start;
		#define kTargetFrameDuration 17
		if (frame_duration<kTargetFrameDuration) SDL_Delay(kTargetFrameDuration-frame_duration);
	}
	
	// redraw original screen before returning
	if (status!=kStatusPowerOff) {
		SDL_FillRect(screen, NULL, 0);
		SDL_Flip(screen);
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