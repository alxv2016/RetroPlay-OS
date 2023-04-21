#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <msettings.h>
#include <stdio.h>
#include <stdlib.h>

#include "../common/controls.h"
#include "../common/defines.h"
#include "../common/interface.h"
#include "../common/keycontext.h"
#include "../common/powerops.h"
#include "../common/utils.h"

#include "settings.h"

/* SETTINGS */
char *menuItems[MENU_ITEMS];
static int volMin = MIN_VOLUME;
static int volMax = MAX_VOLUME;
static int britMin = MIN_BRIGHTNESS;
static int briMax = MAX_BRIGHTNESS;

void settingsMenu(SDL_Surface *surface, int selected, int value, int i) {
#define MIN(a, b) (a) < (b) ? (a) : (b)
  char *item = menuItems[i];
  int rowCount = 4;
  int marginLeft = 32;
  int background = selected == SETTINGS_VOLUMN || selected == SETTINGS_SCREEN
                       ? SDL_MapRGB(surface->format, TRIAD_GRAY)
                       : SDL_MapRGB(surface->format, TRIAD_WHITE);
  SDL_Surface *text;
  text = TTF_RenderUTF8_Blended(g_font.small, item, COLOR_LIGHT_TEXT);
  SDL_Surface *sIcon = i == SETTINGS_SCREEN
                           ? g_gfx.brightness
                           : (i == SETTINGS_VOLUMN ? g_gfx.volume : g_gfx.mute);
  SDL_Surface *sProgressEmpty = g_gfx.settings_bar_empty;
  SDL_Surface *sProgressBar = g_gfx.settings_bar_full;

  int iconMarginLeft = ICON_SIZE + 8;
  int row_width = text->w + marginLeft * 2;
  int volBritWidth = iconMarginLeft + sProgressEmpty->w + marginLeft * 2;
  int max_width = MIN(row_width, 580);
  int row_cy = (ROW_HEIGHT / 2) - (text->h / 2);
  int screen_center = (SCREEN_HEIGHT / 2) - ((ROW_HEIGHT * rowCount) / 2);
  int cy = (ROW_HEIGHT / 2) - (sProgressEmpty->h / 2);
  int w = sProgressBar->w *
          ((float)(GetVolume() - volMin) / (volMax - volMin));
  int h = sProgressBar->h;

  if (i == selected) {
    text = TTF_RenderUTF8_Blended(g_font.small, item, COLOR_DARK_TEXT);
    if (selected == SETTINGS_VOLUMN || selected == SETTINGS_SCREEN) {
      SDL_FillRect(surface,
                   &(SDL_Rect){0, screen_center + i * ROW_HEIGHT, volBritWidth,
                               ROW_HEIGHT},
                   background);
      SDL_BlitSurface(
          sIcon, NULL, surface,
          &(SDL_Rect){marginLeft, screen_center + (i * ROW_HEIGHT) + row_cy});
      SDL_BlitSurface(sProgressEmpty, NULL, surface,
                      &(SDL_Rect){marginLeft + iconMarginLeft,
                                  screen_center + (i * ROW_HEIGHT) + cy});
      SDL_BlitSurface(sProgressBar, &(SDL_Rect){0, 0, w, h}, surface,
                      &(SDL_Rect){marginLeft + iconMarginLeft,
                                  screen_center + (i * ROW_HEIGHT) + cy, w, h});
    } else {
      SDL_FillRect(
          surface,
          &(SDL_Rect){0, screen_center + i * ROW_HEIGHT, max_width, ROW_HEIGHT},
          background);
      SDL_BlitSurface(
          text, &(SDL_Rect){0, 0, max_width, text->h}, surface,
          &(SDL_Rect){marginLeft, screen_center + (i * ROW_HEIGHT) + row_cy});
      SDL_FreeSurface(text);
    }
  } else if (i == SETTINGS_VOLUMN) {
    SDL_BlitSurface(
        sIcon, NULL, surface,
        &(SDL_Rect){marginLeft, screen_center + (i * ROW_HEIGHT) + row_cy});
    SDL_BlitSurface(sProgressEmpty, NULL, surface,
                    &(SDL_Rect){marginLeft + iconMarginLeft,
                                screen_center + (i * ROW_HEIGHT) + cy});
    SDL_BlitSurface(sProgressBar, &(SDL_Rect){0, 0, w, h}, surface,
                    &(SDL_Rect){marginLeft + iconMarginLeft,
                                screen_center + (i * ROW_HEIGHT) + cy, w, h});
  } else if (i == SETTINGS_SCREEN) {
    SDL_BlitSurface(
        sIcon, NULL, surface,
        &(SDL_Rect){marginLeft, screen_center + (i * ROW_HEIGHT) + row_cy});
    SDL_BlitSurface(sProgressEmpty, NULL, surface,
                    &(SDL_Rect){marginLeft + iconMarginLeft,
                                screen_center + (i * ROW_HEIGHT) + cy});
    SDL_BlitSurface(sProgressBar, &(SDL_Rect){0, 0, w, h}, surface,
                    &(SDL_Rect){marginLeft + iconMarginLeft,
                                screen_center + (i * ROW_HEIGHT) + cy, w, h});
  } else {
    SDL_BlitSurface(
        text, &(SDL_Rect){0, 0, max_width, text->h}, surface,
        &(SDL_Rect){marginLeft, screen_center + (i * ROW_HEIGHT) + row_cy});
    SDL_FreeSurface(text);
  }
}

void initSettings(SDL_Surface *surface, int selected, int volValue,int britValue) {
  menuItems[SETTINGS_SLEEP] = "Sleep device";
  menuItems[SETTINGS_POWER] = "Shut down";
  menuItems[SETTINGS_SCREEN] = "Screen Brightness";
  menuItems[SETTINGS_VOLUMN] = "Adjust volumn";

  for (int i = 0; i < MENU_ITEMS; i++) {
    settingsMenu(surface, selected, volValue, i);
  }
}

void showSomeSettings(SDL_Surface *surface) {
  paragraph(surface, "Showing message from settings", 0, 0, SCREEN_WIDTH,
            SCREEN_HEIGHT);
}