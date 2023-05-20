#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <sys/stat.h>
#include <unistd.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>
#include <msettings.h>

#include "../common/defines.h"
#include "../common/keycontext.h"

#include "../common/utils.h"
#include "../common/controls.h"
#include "../common/interface.h"

#include "mmenu.h"

static SDL_Surface* overlay;
static SDL_Surface* screen;

typedef void (*ShowMenu_callback_t)(void);
/////////////////////////////////////////////////////////////
__attribute__((constructor)) static void Menu_init(void) {
	void* librt = dlopen("librt.so.1", RTLD_LAZY | RTLD_GLOBAL); // shm
	void* libmsettings = dlopen("libmsettings.so", RTLD_LAZY | RTLD_GLOBAL);
	InitSettings();
	
	overlay = SDL_CreateRGBSurface(SDL_SWSURFACE, SCREEN_WIDTH, SCREEN_HEIGHT, 16, 0, 0, 0, 0);
	SDL_SetAlpha(overlay, SDL_SRCALPHA, 0x80);
	SDL_FillRect(overlay, NULL, 0);
}

__attribute__((destructor)) static void Menu_quit(void) {
	SDL_FreeSurface(overlay);
}

MenuReturnStatus ShowMenu(char* rom_path, char* save_path_template, SDL_Surface* optional_snapshot, MenuRequestState requested_state, AutoSave_t autosave) {
	screen = SDL_GetVideoSurface();
	SDL_EnableKeyRepeat(300,100);
	SDL_BlitSurface(overlay, NULL, screen, NULL);
	Input_reset();
	
	int status = STATUS_CONTINUE;
	int quit = 0;
	int dirty = 1;

	while (!quit) {
		unsigned long frameStart = SDL_GetTicks();
		Input_poll();

		if ((Input_justPressed(BTN_B) || Input_justReleased(BTN_MENU)) && !dirty) {
			status = STATUS_CONTINUE;
			quit = 1;
		}
		else if (Input_justPressed(BTN_A)) {
			status = STATUS_EXIT;
			quit = 1;
			if (quit) break;
		}

		if (dirty) {
			dirty = 0;
			batteryStatus(gfx.screen, SCREEN_WIDTH - SPACING_LG, 12);
			SDL_Flip(screen);
		}
		
    // slow down to 60fps
    GFX_sync(frameStart);
	}
	
	// redraw original screen before returning
	if (status!=STATUS_POWER) {
		SDL_FillRect(screen, NULL, 0);
		SDL_Flip(screen);
	}


	SDL_EnableKeyRepeat(0,0);
	return status;
}