#include <dlfcn.h>
#include <msettings.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

#include "../common/defines.h"
#include "../common/keycontext.h"

#include "../common/api.h"
#include "../common/controls.h"
#include "../common/interface.h"
#include "../common/utils.h"

#include "mmenu.h"

char *menuItems[MENU_ITEMS];
static int slot = 0;
static int state_support = 1;
static int disable_poweroff = 0;
static SDL_Surface *ingameOverlay;
static SDL_Surface *ingameScreen;

__attribute__((constructor)) static void MMenu_init(void) {
  void *librt = dlopen("librt.so.1", RTLD_LAZY | RTLD_GLOBAL);
  void *libmsettings = dlopen("libmsettings.so", RTLD_LAZY | RTLD_GLOBAL);
  InitSettings();

  menuItems[OPTS_CONTINUE] = "Continue";
  menuItems[OPTS_LOAD] = "Load game";
  menuItems[OPTS_SAVE] = "Save game";
  menuItems[OPTS_SETTINGS] = "Settings";
  menuItems[OPTS_QUIT] = "Quit game";

  ingameOverlay = SDL_CreateRGBSurface(SDL_SWSURFACE, SCREEN_WIDTH, SCREEN_HEIGHT, 16, 0, 0, 0, 0);
  SDL_SetAlpha(ingameOverlay, SDL_SRCALPHA, 0x80);
  SDL_FillRect(ingameOverlay, NULL, 0);
}
__attribute__((destructor)) static void MMenu_quit(void) {
  SDL_FreeSurface(ingameOverlay);
}

/* INGAME MENU */
static void settingActiveItem(SDL_Surface *surface, int y, int w) {
  int background = SDL_MapRGB(surface->format, GREY500);
  int accent = SDL_MapRGB(surface->format, PRIMARY);
  SDL_FillRect(surface, &(SDL_Rect){0, y, w, ROW_HEIGHT}, background);
  SDL_FillRect(surface, &(SDL_Rect){0, y, 6, ROW_HEIGHT}, accent);
}

static void settingItem(SDL_Surface *surface, SDL_Surface *label, int x, int y, int marginLeft, int w) {
  int cy = (ROW_HEIGHT / 2) - (label->h / 2);
  SDL_BlitSurface(label, &(SDL_Rect){0, 0, w, label->h}, surface, &(SDL_Rect){x, y + cy});
  SDL_FreeSurface(label);
}

void settingsMenu(SDL_Surface *surface, int selected, int i) {
#define MIN(min, max) (min) < (max) ? (min) : (max)
  int marginLeft = SPACING_XL;
  char *item = menuItems[i];
  SDL_Surface *label =
      TTF_RenderUTF8_Blended(font.h3, item, (SDL_Color){LIGHT_TEXT});
  int labelMarginLeft = marginLeft + SPACING_MD;
  int labelWidth = labelMarginLeft + label->w + marginLeft;
  int availableWidth = SCREEN_WIDTH / 2 - marginLeft;
  int cy = (ROW_HEIGHT / 2) - (label->h / 2);

  int rowWidth = labelWidth;
  int maxLabelWidth = MIN(rowWidth, labelWidth);
  int screenCenter = (SCREEN_HEIGHT / 2) - ((ROW_HEIGHT * MENU_ITEMS) / 2);

  if (i == selected) {
    settingActiveItem(surface, screenCenter + i * ROW_HEIGHT, rowWidth);
  }
  settingItem(surface, label, labelMarginLeft, screenCenter + (i * ROW_HEIGHT),
              marginLeft, maxLabelWidth);
}

void initSettings(SDL_Surface *surface, int selected) {
  for (int i = 0; i < MENU_ITEMS; i++) {
    settingsMenu(surface, selected, i);
  }
}
/*********/

static MenuReturnStatus SaveLoad(char *rom_path, char *save_path_template, SDL_Surface *optional_snapshot, MenuRequestState requested_state, AutoSave_t autosave) {
  int status = kStatusContinue;
  return status;
}

MenuReturnStatus ShowMenu(char *rom_path, char *save_path_template, SDL_Surface *optional_snapshot, MenuRequestState requested_state, AutoSave_t autosave) {

  ingameScreen = SDL_GetVideoSurface();
  SDL_BlitSurface(ingameOverlay, NULL, ingameScreen, NULL);
  SDL_EnableKeyRepeat(300, 100);
  Input_reset();

  int status = kStatusContinue;
  int optionSelected = 0;
  int quit = 0;
  int dirty = 1;

  while (!quit) {
    unsigned long frameStart = SDL_GetTicks();
    Input_poll();

    if ((Input_justPressed(BTN_B) || Input_justReleased(BTN_MENU)) && !dirty) {
      status = kStatusContinue;
      quit = 1;
    } else if (Input_justPressed(BTN_A)) {
      switch (optionSelected) {
      case OPTS_CONTINUE:
        status = kStatusContinue;
        quit = 1;
        break;
      case OPTS_SAVE:
        break;
      case OPTS_LOAD:
        break;
      case OPTS_SETTINGS:
        status = kStatusOpenMenu;
        quit = 1;
        break;
      case OPTS_QUIT:
        status = kStatusExitGame;
        quit = 1;
        break;
      }
    }

    if (Input_justPressed(BTN_UP)) {
      optionSelected -= 1;
      if (optionSelected < 0) {
        optionSelected += MENU_ITEMS;
      }
      dirty = 1;
    } else if (Input_justPressed(BTN_DOWN)) {
      optionSelected += 1;
      if (optionSelected >= MENU_ITEMS) {
        optionSelected -= MENU_ITEMS;
      }
      dirty = 1;
    }

    if (dirty) {
      SDL_FillRect(ingameScreen, NULL, 0);
      initSettings(ingameScreen, optionSelected);
      SDL_Flip(ingameScreen);
      dirty = 0;
    }

    // slow down to 60fps
    GFX_sync(frameStart);
  }

  if (status != kStatusPowerOff) {
    SDL_FillRect(ingameScreen, NULL, 0);
    SDL_Flip(ingameScreen);
  }

  QuitSettings();
  return status;
}

int ResumeSlot(void) {
  if (!exists(RESUME_SLOT_PATH))
    return -1;
  slot = getInt(RESUME_SLOT_PATH);
  unlink(RESUME_SLOT_PATH);
  return slot;
}

int ChangeDisc(char *disc_path) {
  if (!exists(CHANGE_DISC_PATH))
    return 0;
  getFile(CHANGE_DISC_PATH, disc_path, 256);
  return 1;
}