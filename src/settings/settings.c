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
static int britMax = MAX_BRIGHTNESS;

static int calcProgress(int width, int value, int minValue, int maxValue) {
  int progress = width * ((float)(value - minValue) / (maxValue - minValue));
  return progress;
};

void settingsMenu(SDL_Surface *surface, int selected, int volValue, int britValue, int i) {
#define MIN(a, b) (a) < (b) ? (a) : (b)
  char *item = menuItems[i];
  int rowCount = 4;
  int marginLeft = 32;
  int background = selected == SETTINGS_VOLUMN || selected == SETTINGS_SCREEN? SDL_MapRGB(surface->format, TRIAD_GRAY): SDL_MapRGB(surface->format, TRIAD_WHITE);
  SDL_Surface *text;
  text = TTF_RenderUTF8_Blended(g_font.small, item, COLOR_LIGHT_TEXT);
  SDL_Surface *sIcon;
  SDL_Surface *sProgressEmpty = g_gfx.settings_bar_empty;
  SDL_Surface *sProgressBarVol = g_gfx.settings_bar_full;
  SDL_Surface *sProgressBarBrit = g_gfx.settings_bar_full;

  switch(i) {
    case SETTINGS_SCREEN:
    sIcon = g_gfx.brightness;
    break;
    case SETTINGS_VOLUMN:
    sIcon = g_gfx.volume;
    volValue == 0 ? sIcon = g_gfx.mute : g_gfx.volume;
    break;
  }

  int iconMarginLeft = ICON_SIZE + 8;
  int row_width = text->w + marginLeft * 2;
  int volBritWidth = iconMarginLeft + sProgressEmpty->w + marginLeft * 2;
  int max_width = MIN(row_width, 580);
  int row_cy = (ROW_HEIGHT / 2) - (text->h / 2);
  int screen_center = (SCREEN_HEIGHT / 2) - ((ROW_HEIGHT * rowCount) / 2);
  int cy = (ROW_HEIGHT / 2) - (sProgressEmpty->h / 2);

  int volProgressW = calcProgress(sProgressBarVol->w, volValue, volMin, volMax);
  int britProgressW = calcProgress(sProgressBarBrit->w, britValue, britMin, britMax);
  int h = sProgressEmpty->h;

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
     if (selected == SETTINGS_VOLUMN) {
       SDL_BlitSurface(sProgressBarVol, &(SDL_Rect){0, 0, volProgressW, h}, surface,
                      &(SDL_Rect){marginLeft + iconMarginLeft,
                                  screen_center + (i * ROW_HEIGHT) + cy, volProgressW, h});
     } else {
       SDL_BlitSurface(sProgressBarBrit, &(SDL_Rect){0, 0, britProgressW, h}, surface,
                      &(SDL_Rect){marginLeft + iconMarginLeft,
                                  screen_center + (i * ROW_HEIGHT) + cy, britProgressW, h});
     }
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
    SDL_BlitSurface(sProgressBarVol, &(SDL_Rect){0, 0, volProgressW, h}, surface,
                    &(SDL_Rect){marginLeft + iconMarginLeft,
                                screen_center + (i * ROW_HEIGHT) + cy, volProgressW, h});
  } else if (i == SETTINGS_SCREEN) {
    SDL_BlitSurface(
        sIcon, NULL, surface,
        &(SDL_Rect){marginLeft, screen_center + (i * ROW_HEIGHT) + row_cy});
    SDL_BlitSurface(sProgressEmpty, NULL, surface,
                    &(SDL_Rect){marginLeft + iconMarginLeft,
                                screen_center + (i * ROW_HEIGHT) + cy});
    SDL_BlitSurface(sProgressBarBrit, &(SDL_Rect){0, 0, britProgressW, h}, surface,
                    &(SDL_Rect){marginLeft + iconMarginLeft,
                                screen_center + (i * ROW_HEIGHT) + cy, britProgressW, h});
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
    settingsMenu(surface, selected, volValue, britValue, i);
  }
}