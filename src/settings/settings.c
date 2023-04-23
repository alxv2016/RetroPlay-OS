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
int timerItems[TIMER_ITEMS] = {0, 15, 25, 30};
static int volMin = MIN_VOLUME;
static int volMax = MAX_VOLUME;
static int britMin = MIN_BRIGHTNESS;
static int britMax = MAX_BRIGHTNESS;

static int calcProgress(int width, int value, int minValue, int maxValue) {
  int progress = width * ((float)(value - minValue) / (maxValue - minValue));
  return progress;
};

void setSleepTime(int value) {
  putInt(CONFIG_PATH, value);
}

int getSleepTime(void) {
  int time = getInt(CONFIG_PATH);
  return time ? time: 0;
}

void settingsMenu(SDL_Surface *surface, int selected, int volValue, int britValue, int i) {
#define MIN(a, b) (a) < (b) ? (a) : (b)
  char *item = menuItems[i];
  int rowCount = MENU_ITEMS;
  int marginLeft = 32;
  int background = selected == SETTINGS_VOLUMN || selected == SETTINGS_SCREEN
                       ? SDL_MapRGB(surface->format, TRIAD_ACTIVE)
                       : SDL_MapRGB(surface->format, TRIAD_ACTIVE);
  int accent = SDL_MapRGB(surface->format, TRIAD_ACCENT);
  SDL_Surface *text;
  text = TTF_RenderUTF8_Blended(g_font.small, item, COLOR_LIGHT_TEXT);
  SDL_Surface *powerIcon = g_gfx.power;
  SDL_Surface *sleepIcon = g_gfx.sleep;
  SDL_Surface *sleepTimerIcon = g_gfx.sleep_timer;
  SDL_Surface *britIcon = g_gfx.brightness;
  SDL_Surface *volIcon = volValue == 0? g_gfx.mute: g_gfx.volume;
  SDL_Surface *progressEmptyBar = g_gfx.settings_bar_empty;
  SDL_Surface *progressVolBar = g_gfx.settings_bar_full;
  SDL_Surface *progressBritBar = g_gfx.settings_bar_full;

  int iconTotalWidth = ICON_SIZE + 24;
  int progressTotalWidth = iconTotalWidth + progressEmptyBar->w + marginLeft;
  int rowWidth = text->w + marginLeft * 2;
  int maxLabelWidth = MIN(rowWidth, 580);
  int rowCY = (ROW_HEIGHT / 2) - (text->h / 2);
  int screenCenter = (SCREEN_HEIGHT / 2) - ((ROW_HEIGHT * rowCount) / 2);
  int cy = (ROW_HEIGHT / 2) - (progressEmptyBar->h / 2);
  int h = progressEmptyBar->h;

  int progressVol = calcProgress(progressVolBar->w, volValue, volMin, volMax);
  int progressBrit = calcProgress(progressBritBar->w, britValue, britMin, britMax);

  if (i == selected) {
     if (selected == SETTINGS_VOLUMN) {
        SDL_FillRect(surface,&(SDL_Rect){0, screenCenter + i * ROW_HEIGHT, iconTotalWidth + progressTotalWidth, ROW_HEIGHT},background);
        SDL_FillRect(surface,&(SDL_Rect){0, screenCenter + i * ROW_HEIGHT, 6, ROW_HEIGHT}, accent);
        SDL_BlitSurface(volIcon, NULL, surface, &(SDL_Rect){marginLeft, screenCenter + (i * ROW_HEIGHT) + rowCY});
        SDL_BlitSurface(progressEmptyBar, NULL, surface, &(SDL_Rect){marginLeft + iconTotalWidth, screenCenter + (i * ROW_HEIGHT) + cy});
        SDL_BlitSurface(progressVolBar, &(SDL_Rect){0, 0, progressVol, h}, surface, &(SDL_Rect){marginLeft + iconTotalWidth, screenCenter + (i * ROW_HEIGHT) + cy, progressVol, h});
     } else if (selected == SETTINGS_SCREEN) {
        SDL_FillRect(surface,&(SDL_Rect){0, screenCenter + i * ROW_HEIGHT, iconTotalWidth + progressTotalWidth, ROW_HEIGHT},background);
        SDL_FillRect(surface,&(SDL_Rect){0, screenCenter + i * ROW_HEIGHT, 6, ROW_HEIGHT}, accent);
        SDL_BlitSurface(britIcon, NULL, surface, &(SDL_Rect){marginLeft, screenCenter + (i * ROW_HEIGHT) + rowCY});
        SDL_BlitSurface(progressEmptyBar, NULL, surface, &(SDL_Rect){marginLeft + iconTotalWidth, screenCenter + (i * ROW_HEIGHT) + cy});
        SDL_BlitSurface(progressBritBar, &(SDL_Rect){0, 0, progressBrit, h}, surface, &(SDL_Rect){marginLeft + iconTotalWidth, screenCenter + (i * ROW_HEIGHT) + cy, progressBrit, h}); 
     } else if (selected == SETTINGS_POWER) {
        SDL_FillRect(surface,&(SDL_Rect){0, screenCenter + i * ROW_HEIGHT, iconTotalWidth + maxLabelWidth, ROW_HEIGHT},background);
        SDL_FillRect(surface,&(SDL_Rect){0, screenCenter + i * ROW_HEIGHT, 6, ROW_HEIGHT}, accent);
        SDL_BlitSurface(powerIcon, NULL, surface, &(SDL_Rect){marginLeft, screenCenter + (i * ROW_HEIGHT) + rowCY});
        SDL_BlitSurface(text, &(SDL_Rect){0, 0, iconTotalWidth + maxLabelWidth, text->h}, surface, &(SDL_Rect){marginLeft + iconTotalWidth, screenCenter + (i * ROW_HEIGHT) + rowCY});
     } else if (selected == SETTINGS_SLEEP) {
        SDL_FillRect(surface,&(SDL_Rect){0, screenCenter + i * ROW_HEIGHT, iconTotalWidth + maxLabelWidth, ROW_HEIGHT},background);
        SDL_FillRect(surface,&(SDL_Rect){0, screenCenter + i * ROW_HEIGHT, 6, ROW_HEIGHT}, accent);
        SDL_BlitSurface(sleepIcon, NULL, surface, &(SDL_Rect){marginLeft, screenCenter + (i * ROW_HEIGHT) + rowCY});
        SDL_BlitSurface(text, &(SDL_Rect){0, 0,iconTotalWidth + maxLabelWidth, text->h}, surface, &(SDL_Rect){marginLeft + iconTotalWidth, screenCenter + (i * ROW_HEIGHT) + rowCY});
     } else if (selected == SETTINGS_SLEEPTIME) {
        SDL_FillRect(surface,&(SDL_Rect){0, screenCenter + i * ROW_HEIGHT, iconTotalWidth + maxLabelWidth, ROW_HEIGHT},background);
        SDL_FillRect(surface,&(SDL_Rect){0, screenCenter + i * ROW_HEIGHT, 6, ROW_HEIGHT}, accent);
        SDL_BlitSurface(sleepTimerIcon, NULL, surface, &(SDL_Rect){marginLeft, screenCenter + (i * ROW_HEIGHT) + rowCY});
        SDL_BlitSurface(text, &(SDL_Rect){0, 0,iconTotalWidth + maxLabelWidth, text->h}, surface, &(SDL_Rect){marginLeft + iconTotalWidth, screenCenter + (i * ROW_HEIGHT) + rowCY});
     } 
    SDL_FreeSurface(text);
  } else if (i == SETTINGS_VOLUMN) {
    SDL_BlitSurface(volIcon, NULL, surface, &(SDL_Rect){marginLeft, screenCenter + (i * ROW_HEIGHT) + rowCY});
    SDL_BlitSurface(progressEmptyBar, NULL, surface, &(SDL_Rect){marginLeft + iconTotalWidth, screenCenter + (i * ROW_HEIGHT) + cy});
    SDL_BlitSurface(progressVolBar, &(SDL_Rect){0, 0, progressVol, h}, surface, &(SDL_Rect){marginLeft + iconTotalWidth, screenCenter + (i * ROW_HEIGHT) + cy, progressVol, h});
  } else if (i == SETTINGS_SCREEN) {
    SDL_BlitSurface(britIcon, NULL, surface, &(SDL_Rect){marginLeft, screenCenter + (i * ROW_HEIGHT) + rowCY});
    SDL_BlitSurface(progressEmptyBar, NULL, surface, &(SDL_Rect){marginLeft + iconTotalWidth, screenCenter + (i * ROW_HEIGHT) + cy});
    SDL_BlitSurface(progressBritBar, &(SDL_Rect){0, 0, progressBrit, h}, surface, &(SDL_Rect){marginLeft + iconTotalWidth, screenCenter + (i * ROW_HEIGHT) + cy, progressBrit, h}); 
  } else if (i == SETTINGS_POWER) {
    SDL_BlitSurface(powerIcon, NULL, surface, &(SDL_Rect){marginLeft, screenCenter + (i * ROW_HEIGHT) + rowCY});
    SDL_BlitSurface(text, &(SDL_Rect){0, 0, iconTotalWidth + maxLabelWidth, text->h}, surface, &(SDL_Rect){marginLeft + iconTotalWidth, screenCenter + (i * ROW_HEIGHT) + rowCY});
    SDL_FreeSurface(text);
  } else if (i == SETTINGS_SLEEP) {
    SDL_BlitSurface(sleepIcon, NULL, surface, &(SDL_Rect){marginLeft, screenCenter + (i * ROW_HEIGHT) + rowCY});
    SDL_BlitSurface(text, &(SDL_Rect){0, 0, iconTotalWidth + maxLabelWidth, text->h}, surface, &(SDL_Rect){marginLeft + iconTotalWidth, screenCenter + (i * ROW_HEIGHT) + rowCY});
    SDL_FreeSurface(text);
  } else if (i == SETTINGS_SLEEPTIME) {
    SDL_BlitSurface(sleepTimerIcon, NULL, surface, &(SDL_Rect){marginLeft, screenCenter + (i * ROW_HEIGHT) + rowCY});
    SDL_BlitSurface(text, &(SDL_Rect){0, 0, iconTotalWidth + maxLabelWidth, text->h}, surface, &(SDL_Rect){marginLeft + iconTotalWidth, screenCenter + (i * ROW_HEIGHT) + rowCY});
    SDL_FreeSurface(text);
  }

}

void initSettings(SDL_Surface *surface, int selected, int volValue, int britValue, int sleepValue) {
  setSleepTime(sleepValue);
  char sleepTimer[256];
  sprintf(sleepTimer, "Sleep timer: %imin", timerItems[sleepValue]);
  menuItems[SETTINGS_SLEEP] = "Sleep mode";
  menuItems[SETTINGS_POWER] = "Power off";
  menuItems[SETTINGS_SLEEPTIME] = sleepTimer;
  menuItems[SETTINGS_SCREEN] = "Brightness";
  menuItems[SETTINGS_VOLUMN] = "Volumn";

  SDL_Surface *firmwareInfo;
  char firmware[256];
  sprintf(firmware, "Firmware: %s", getenv("MIYOO_VERSION"));
  firmwareInfo = TTF_RenderUTF8_Blended(g_font.footnote, firmware, COLOR_GRAY200);
  SDL_BlitSurface(firmwareInfo, NULL, surface, &(SDL_Rect){32, SCREEN_HEIGHT - ((ICON_SIZE / 2) + 20)});
  SDL_FreeSurface(firmwareInfo);
  for (int i = 0; i < MENU_ITEMS; i++) {
    settingsMenu(surface, selected, volValue, britValue, i);
  }
}