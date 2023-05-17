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
#include "../common/powerops.h"
#include "../common/rumble.h"
#include "../common/settings.h"

#include "mmenu.h"

static int game_slot = 0;
static int state_support = 1;
static int disable_poweroff = 0;
int show_menu = 0;

typedef void (*ShowMenu_callback_t)(void);
/////////////////////////////////////////////////////////////
static void Menu_init(void) {
	void* librt = dlopen("librt.so.1", RTLD_LAZY | RTLD_GLOBAL); // shm
	void* libmsettings = dlopen("libmsettings.so", RTLD_LAZY | RTLD_GLOBAL);
	InitSettings();
	GFX_init();
	gfx.overlay = SDL_CreateRGBSurface(SDL_SWSURFACE, SCREEN_WIDTH, SCREEN_HEIGHT, 16, 0, 0, 0, 0);
	SDL_SetAlpha(gfx.overlay, SDL_SRCALPHA, 0x80);
	SDL_FillRect(gfx.overlay, NULL, 0);
}

static void Menu_quit(void) {
	// GFX_quit();
}

static void Menu_init() __attribute__((constructor));
static void Menu_quit() __attribute__((destructor));

void SystemRequest(char* rom_path, int request, AutoSave_t autosave) {
	autosave();
	putFile(AUTO_RESUME_PATH, rom_path + strlen(SDCARD_PATH));
	if (request==REQ_SLEEP) {
		fauxSleep();
		unlink(AUTO_RESUME_PATH);
	}
	else powerOff();
}

void ShowWarning(void) {
	state_support = 0;
	disable_poweroff = 1;
	disablePoweroff();
	
	gfx.screen = SDL_GetVideoSurface();
	GFX_ready();

	SDL_Flip(gfx.screen);
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

MenuReturnStatus ShowMenu(char* rom_path, char* save_path_template, SDL_Surface* optional_snapshot, int requested_state, AutoSave_t autosave) {
  show_menu = 1;
	gfx.screen = SDL_GetVideoSurface();
	GFX_init();
	GFX_ready();
	SDL_EnableKeyRepeat(300,100);

	// int copy_bytes = screen->h * screen->pitch;
	// void* copy_pixels = malloc(copy_bytes);
	// memcpy(copy_pixels, screen->pixels, copy_bytes);
	// SDL_Surface* copy = SDL_CreateRGBSurfaceFrom(copy_pixels, screen->w,screen->h, screen->format->BitsPerPixel, screen->pitch, screen->format->Rmask, screen->format->Gmask, screen->format->Bmask, screen->format->Amask);
	// SDL_Surface* cache = SDL_CreateRGBSurface(SDL_SWSURFACE, SCREEN_WIDTH, SCREEN_HEIGHT, 16, 0, 0, 0, 0);
	// SDL_BlitSurface(copy, NULL, cache, NULL);
	// SDL_BlitSurface(gfx.overlay, NULL, cache, NULL);

  emptyState(gfx.screen, gfx.empty_state, H2, CAPTION, "MMENU", "Loaded successfully.");


	Input_reset();
	int status = STATUS_CONTINUE;
	int show_menu = 0;
	int dirty = 1;

  	// sleep or poweroff
	if (requested_state!=REQ_MENU) {
		if (disable_poweroff && requested_state==REQ_POWER) return STATUS_CONTINUE;
		SystemRequest(rom_path, requested_state, autosave);
		if (requested_state==REQ_POWER) return REQ_POWER;
	}

	while(!show_menu) {
		unsigned long frame_start = SDL_GetTicks();
		Input_poll();

		if ((Input_justPressed(BTN_B) || Input_justReleased(BTN_MENU)) && !dirty) {
			status = STATUS_CONTINUE;
			show_menu = 1;
		}
		
		if (Input_justPressed(BTN_X)) {
			status = STATUS_EXIT;
			show_menu = 1;
		}
		if (Input_justPressed(BTN_Y)) {
			status = STATUS_OPTIONS;
			show_menu = 1;
		}

		if (show_menu) break;

		// Show menu UI
		if(dirty) {
			dirty = 0;
			// SDL_BlitSurface(cache, NULL, screen, NULL);
			// GFX_blitBattery(screen, 576, 12);

			SDL_Flip(screen);
		}

		// slow down to 60fps
		unsigned long frame_duration = SDL_GetTicks() - frame_start;
		#define FRAME_DURATION 17
		if (frame_duration<FRAME_DURATION) SDL_Delay(FRAME_DURATION-frame_duration);
	}

	if (status!=STATUS_POWER) {
		SDL_FillRect(screen, NULL, 0);
		// SDL_BlitSurface(copy, NULL, screen, NULL);
		SDL_Flip(screen);
	}

	// SDL_FreeSurface(cache);
	// SDL_FreeSurface(copy);
	// free(copy_pixels); 
	SDL_EnableKeyRepeat(0,0);
  return status;
}