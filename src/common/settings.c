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

void setSleepTime(int value) {
  putInt(CONFIG_PATH, value);
}

int getSleepTime(void) {
  int time = getInt(CONFIG_PATH);
  return time ? time: 0;
}

static void settingActiveItem(SDL_Surface *surface, int y, int w) {
  int background = SDL_MapRGB(surface->format, GREY500);
  int accent = SDL_MapRGB(surface->format, PRIMARY);
  SDL_FillRect(surface, &(SDL_Rect){0, y, w, ROW_HEIGHT},background);
  SDL_FillRect(surface, &(SDL_Rect){0, y, 6, ROW_HEIGHT}, accent);
}

static void settingItem(SDL_Surface *surface, SDL_Surface *icon, SDL_Surface *label, int x, int y, int marginLeft, int w) {
  int cy = (ROW_HEIGHT / 2) - (label->h / 2);
  SDL_BlitSurface(icon, NULL, surface, &(SDL_Rect){marginLeft, y + cy});
  SDL_BlitSurface(label, &(SDL_Rect){0, 0, w, label->h}, surface, &(SDL_Rect){x, y + cy});
  SDL_FreeSurface(label);
}

void settingsMenu(SDL_Surface *surface, int selected, int volValue, int britValue, int i) {
  #define MIN(min, max) (min) < (max) ? (min) : (max)
  SDL_Surface *powerIcon = gfx.power;
  SDL_Surface *sleepIcon = gfx.sleep;
  SDL_Surface *sleepTimerIcon = gfx.sleep_timer;
  int marginLeft = SPACING_XL;
  char *item = menuItems[i];
  SDL_Surface *label = TTF_RenderUTF8_Blended(font.h3, item, (SDL_Color){LIGHT_TEXT});
  int labelMarginLeft = marginLeft + ICON_SIZE + SPACING_MD;
  int labelWidth = labelMarginLeft + label->w + marginLeft;
  int availableWidth = SCREEN_WIDTH / 2 - marginLeft;
  int cy = (ROW_HEIGHT / 2) - (label->h / 2);

  int rowWidth = i == SETTINGS_VOLUMN || i == SETTINGS_SCREEN? labelMarginLeft + availableWidth + marginLeft : labelWidth;
  int maxLabelWidth = MIN(rowWidth, labelWidth);
  int screenCenter = (SCREEN_HEIGHT / 2) - ((ROW_HEIGHT * MENU_ITEMS) / 2);

  if (i == selected) {
     if (selected == SETTINGS_VOLUMN) {
        settingActiveItem(surface, screenCenter + i * ROW_HEIGHT, rowWidth);
        volumeControl(surface, marginLeft, screenCenter + (i * ROW_HEIGHT) + cy, 0, volValue, volMin, volMax);
     } else if (selected == SETTINGS_SCREEN) {
        settingActiveItem(surface, screenCenter + i * ROW_HEIGHT, rowWidth);
        brightnessControl(surface, marginLeft, screenCenter + (i * ROW_HEIGHT) + cy, britValue, britMin, britMax);
     } else if (selected == SETTINGS_POWER) {
        settingActiveItem(surface, screenCenter + i * ROW_HEIGHT, rowWidth);
        settingItem(surface, powerIcon, label, labelMarginLeft, screenCenter + (i * ROW_HEIGHT), marginLeft, maxLabelWidth);
     } else if (selected == SETTINGS_SLEEP) {
        settingActiveItem(surface, screenCenter + i * ROW_HEIGHT, rowWidth);
        settingItem(surface, sleepIcon, label, labelMarginLeft, screenCenter + (i * ROW_HEIGHT), marginLeft, maxLabelWidth);
     } else if (selected == SETTINGS_SLEEPTIME) {
        settingActiveItem(surface, screenCenter + i * ROW_HEIGHT, rowWidth);
        settingItem(surface, sleepTimerIcon, label, labelMarginLeft, screenCenter + (i * ROW_HEIGHT), marginLeft, maxLabelWidth);
     } 
  } else if (i == SETTINGS_VOLUMN) {
    volumeControl(surface, marginLeft, screenCenter + (i * ROW_HEIGHT) + cy, 0, volValue, volMin, volMax);
  } else if (i == SETTINGS_SCREEN) {
    brightnessControl(surface, marginLeft, screenCenter + (i * ROW_HEIGHT) + cy, britValue, britMin, britMax);
  } else if (i == SETTINGS_POWER) {
    settingItem(surface, powerIcon, label, labelMarginLeft, screenCenter + (i * ROW_HEIGHT), marginLeft, maxLabelWidth);
  } else if (i == SETTINGS_SLEEP) {
    settingItem(surface, sleepIcon, label, labelMarginLeft, screenCenter + (i * ROW_HEIGHT), marginLeft, maxLabelWidth);
  } else if (i == SETTINGS_SLEEPTIME) {
    settingItem(surface, sleepTimerIcon, label, labelMarginLeft, screenCenter + (i * ROW_HEIGHT), marginLeft, maxLabelWidth);
  }

}

void initSettings(SDL_Surface *surface, int selected, int volValue, int britValue, int sleepValue) {
  setSleepTime(sleepValue);
  int currentTimer = getSleepTime();
  char sleepLabel[14] = "Sleep timer:";
  char sleepTimer[256];
  currentTimer > 0 ? sprintf(sleepTimer, "%s %imins",sleepLabel, timerItems[sleepValue]) : sprintf(sleepTimer, "%s Never", sleepLabel);
  menuItems[SETTINGS_SLEEP] = "Sleep mode";
  menuItems[SETTINGS_POWER] = "Power off";
  menuItems[SETTINGS_SLEEPTIME] = sleepTimer;
  menuItems[SETTINGS_SCREEN] = "Brightness";
  menuItems[SETTINGS_VOLUMN] = "Volumn";

  SDL_Surface *firmwareInfo;
  char firmware[256];
  sprintf(firmware, "Firmware: %s", getenv("MIYOO_VERSION"));
  firmwareInfo = TTF_RenderUTF8_Blended(font.footnote, firmware, (SDL_Color){NEUTRAL_TEXT});
  int cy = (ICON_SIZE / 2) - (firmwareInfo->h / 2);
  SDL_BlitSurface(firmwareInfo, NULL, surface, &(SDL_Rect){SPACING_XL, SCREEN_HEIGHT - SPACING_LG + cy - ICON_SIZE});
  SDL_FreeSurface(firmwareInfo);
  for (int i = 0; i < MENU_ITEMS; i++) {
    settingsMenu(surface, selected, volValue, britValue, i);
  }
}