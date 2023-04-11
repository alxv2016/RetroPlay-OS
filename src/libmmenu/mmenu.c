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

// #include "mmenu.h"

///////////////////////////////////////////////////////////////
// Video screen surface
static SDL_Surface* screen;
static SDL_Surface* overlay;
///////////////////////////////////////////////////////////////
enum MenuReturnStatus {
	kStatusContinue = 0,
	kStatusSaveSlot = 1,
	kStatusLoadSlot = 11,
	kStatusOpenMenu = 23,
	kStatusChangeDisc = 24,
	kStatusResetGame = 25,
	kStatusExitGame = 30,
	kStatusPowerOff = 31,
};

typedef void (*ShowMenu_callback_t)(void);
/////////////////////////////////////////////////////////////
static void Menu_init(void) {
	void* librt = dlopen("librt.so.1", RTLD_LAZY | RTLD_GLOBAL); // shm
	void* libmsettings = dlopen("libmsettings.so", RTLD_LAZY | RTLD_GLOBAL);
	InitSettings();
	GFX_init();
	overlay = SDL_CreateRGBSurface(SDL_SWSURFACE, SCREEN_WIDTH, SCREEN_HEIGHT, 16, 0, 0, 0, 0);
	SDL_SetAlpha(overlay, SDL_SRCALPHA, 0x80);
	SDL_FillRect(overlay, NULL, 0);
}

static void Menu_quit(void) {
	SDL_FreeSurface(overlay);
}

static void Menu_init() __attribute__((constructor));
static void Menu_quit() __attribute__((destructor));

void ShowMenu() {
	screen = SDL_GetVideoSurface();
	GFX_init();
	GFX_ready();
	SDL_EnableKeyRepeat(300,100);

	int copy_bytes = screen->h * screen->pitch;
	void* copy_pixels = malloc(copy_bytes);
	memcpy(copy_pixels, screen->pixels, copy_bytes);
	SDL_Surface* copy = SDL_CreateRGBSurfaceFrom(copy_pixels, screen->w,screen->h, screen->format->BitsPerPixel, screen->pitch, screen->format->Rmask, screen->format->Gmask, screen->format->Bmask, screen->format->Amask);
	SDL_Surface* cache = SDL_CreateRGBSurface(SDL_SWSURFACE, SCREEN_WIDTH, SCREEN_HEIGHT, 16, 0, 0, 0, 0);
	SDL_BlitSurface(copy, NULL, cache, NULL);
	SDL_BlitSurface(overlay, NULL, cache, NULL);

	Input_reset();
	int status = kStatusContinue;

	int show_menu = 0;
	int dirty = 1;

	while(!show_menu) {
		unsigned long frame_start = SDL_GetTicks();

		Input_poll();
		if ((Input_justPressed(BTN_B) || Input_justReleased(BTN_MENU)) && !dirty) {
			status = kStatusContinue;
			show_menu = 1;
		}
		
		if (Input_justPressed(BTN_X)) {
			status = kStatusExitGame;
			show_menu = 1;
		}
		if (Input_justPressed(BTN_Y)) {
			status = kStatusOpenMenu;
			show_menu = 1;
		}

		if (show_menu) break;

		// Show menu UI
		if(dirty) {
			dirty = 0;
			SDL_BlitSurface(cache, NULL, screen, NULL);
			// GFX_blitBattery(screen, 576, 12);


			SDL_Flip(screen);
		}

		// slow down to 60fps
		unsigned long frame_duration = SDL_GetTicks() - frame_start;
		#define FRAME_DURATION 17
		if (frame_duration<FRAME_DURATION) SDL_Delay(FRAME_DURATION-frame_duration);
	}

	if (status!=kStatusPowerOff) {
		SDL_FillRect(screen, NULL, 0);
		SDL_BlitSurface(copy, NULL, screen, NULL);
		SDL_Flip(screen);
	}

	SDL_FreeSurface(cache);
	SDL_FreeSurface(copy);
	free(copy_pixels); 
	SDL_EnableKeyRepeat(0,0);
	// return status;
}
