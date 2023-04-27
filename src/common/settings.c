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

void settingsMenu(SDL_Surface *surface, int selected, int volValue, int britValue, int i) {
  #define MIN(min, max) (min) < (max) ? (min) : (max)
  SDL_Surface *powerIcon = gfx.power;
  SDL_Surface *sleepIcon = gfx.sleep;
  SDL_Surface *sleepTimerIcon = gfx.sleep_timer;
  char *item = menuItems[i];
  int rowCount = MENU_ITEMS;
  int marginLeft = SPACING_XL;
  int labelMarginLeft = marginLeft + ICON_SIZE + SPACING_MD;
  SDL_Surface *text = TTF_RenderUTF8_Blended(font.h3, item, (SDL_Color){LIGHT_TEXT});
  int labelWidth = labelMarginLeft + text->w + marginLeft;
  int background = SDL_MapRGB(surface->format, GREY500);
  int accent = SDL_MapRGB(surface->format, PRIMARY);
  int w = SCREEN_WIDTH / 2 - marginLeft;

  int rowWidth = i == SETTINGS_VOLUMN || i == SETTINGS_SCREEN? labelMarginLeft + w + marginLeft : labelWidth;
  int maxLabelWidth = MIN(rowWidth, labelWidth);
  int rowCY = (ROW_HEIGHT / 2) - (text->h / 2);
  int screenCenter = (SCREEN_HEIGHT / 2) - ((ROW_HEIGHT * rowCount) / 2);

  if (i == selected) {
     if (selected == SETTINGS_VOLUMN) {
        SDL_FillRect(surface,&(SDL_Rect){0, screenCenter + i * ROW_HEIGHT, rowWidth, ROW_HEIGHT},background);
        SDL_FillRect(surface,&(SDL_Rect){0, screenCenter + i * ROW_HEIGHT, 6, ROW_HEIGHT}, accent);
        volumeControl(surface, marginLeft, screenCenter + (i * ROW_HEIGHT) + rowCY, volValue, volMin, volMax);
     } else if (selected == SETTINGS_SCREEN) {
        SDL_FillRect(surface,&(SDL_Rect){0, screenCenter + i * ROW_HEIGHT, rowWidth, ROW_HEIGHT},background);
        SDL_FillRect(surface,&(SDL_Rect){0, screenCenter + i * ROW_HEIGHT, 6, ROW_HEIGHT}, accent);
        brightnessControl(surface, marginLeft, screenCenter + (i * ROW_HEIGHT) + rowCY, britValue, britMin, britMax);
     } else if (selected == SETTINGS_POWER) {
        SDL_FillRect(surface,&(SDL_Rect){0, screenCenter + i * ROW_HEIGHT, rowWidth, ROW_HEIGHT},background);
        SDL_FillRect(surface,&(SDL_Rect){0, screenCenter + i * ROW_HEIGHT, 6, ROW_HEIGHT}, accent);
        SDL_BlitSurface(powerIcon, NULL, surface, &(SDL_Rect){marginLeft, screenCenter + (i * ROW_HEIGHT) + rowCY});
        SDL_BlitSurface(text, &(SDL_Rect){0, 0, maxLabelWidth, text->h}, surface, &(SDL_Rect){labelMarginLeft, screenCenter + (i * ROW_HEIGHT) + rowCY});
     } else if (selected == SETTINGS_SLEEP) {
        SDL_FillRect(surface,&(SDL_Rect){0, screenCenter + i * ROW_HEIGHT, rowWidth, ROW_HEIGHT},background);
        SDL_FillRect(surface,&(SDL_Rect){0, screenCenter + i * ROW_HEIGHT, 6, ROW_HEIGHT}, accent);
        SDL_BlitSurface(sleepIcon, NULL, surface, &(SDL_Rect){marginLeft, screenCenter + (i * ROW_HEIGHT) + rowCY});
        SDL_BlitSurface(text, &(SDL_Rect){0, 0, maxLabelWidth, text->h}, surface, &(SDL_Rect){labelMarginLeft, screenCenter + (i * ROW_HEIGHT) + rowCY});
     } else if (selected == SETTINGS_SLEEPTIME) {
        SDL_FillRect(surface,&(SDL_Rect){0, screenCenter + i * ROW_HEIGHT, rowWidth, ROW_HEIGHT},background);
        SDL_FillRect(surface,&(SDL_Rect){0, screenCenter + i * ROW_HEIGHT, 6, ROW_HEIGHT}, accent);
        SDL_BlitSurface(sleepTimerIcon, NULL, surface, &(SDL_Rect){marginLeft, screenCenter + (i * ROW_HEIGHT) + rowCY});
        SDL_BlitSurface(text, &(SDL_Rect){0, 0, maxLabelWidth, text->h}, surface, &(SDL_Rect){labelMarginLeft, screenCenter + (i * ROW_HEIGHT) + rowCY});
     } 
    SDL_FreeSurface(text);
  } else if (i == SETTINGS_VOLUMN) {
    volumeControl(surface, marginLeft, screenCenter + (i * ROW_HEIGHT) + rowCY, volValue, volMin, volMax);
  } else if (i == SETTINGS_SCREEN) {
    brightnessControl(surface, marginLeft, screenCenter + (i * ROW_HEIGHT) + rowCY, britValue, britMin, britMax);
  } else if (i == SETTINGS_POWER) {
    SDL_BlitSurface(powerIcon, NULL, surface, &(SDL_Rect){marginLeft, screenCenter + (i * ROW_HEIGHT) + rowCY});
    SDL_BlitSurface(text, &(SDL_Rect){0, 0, maxLabelWidth, text->h}, surface, &(SDL_Rect){labelMarginLeft, screenCenter + (i * ROW_HEIGHT) + rowCY});
    SDL_FreeSurface(text);
  } else if (i == SETTINGS_SLEEP) {
    SDL_BlitSurface(sleepIcon, NULL, surface, &(SDL_Rect){marginLeft, screenCenter + (i * ROW_HEIGHT) + rowCY});
    SDL_BlitSurface(text, &(SDL_Rect){0, 0, maxLabelWidth, text->h}, surface, &(SDL_Rect){labelMarginLeft, screenCenter + (i * ROW_HEIGHT) + rowCY});
    SDL_FreeSurface(text);
  } else if (i == SETTINGS_SLEEPTIME) {
    SDL_BlitSurface(sleepTimerIcon, NULL, surface, &(SDL_Rect){marginLeft, screenCenter + (i * ROW_HEIGHT) + rowCY});
    SDL_BlitSurface(text, &(SDL_Rect){0, 0, maxLabelWidth, text->h}, surface, &(SDL_Rect){labelMarginLeft, screenCenter + (i * ROW_HEIGHT) + rowCY});
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
  firmwareInfo = TTF_RenderUTF8_Blended(font.footnote, firmware, (SDL_Color){NEUTRAL_TEXT});
  SDL_BlitSurface(firmwareInfo, NULL, surface, &(SDL_Rect){32, SCREEN_HEIGHT - ((ICON_SIZE / 2) + 20)});
  SDL_FreeSurface(firmwareInfo);
  for (int i = 0; i < MENU_ITEMS; i++) {
    settingsMenu(surface, selected, volValue, britValue, i);
  }
}