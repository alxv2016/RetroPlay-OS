#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <msettings.h>
#include <stdio.h>
#include <stdlib.h>

#include "../common/defines.h"
#include "../common/keycontext.h"
#include "../common/interface.h"
#include "../common/utils.h"
#include "../common/controls.h"
#include "../common/powerops.h"

#include "settings.h"

/* SETTINGS */
#define MENU_ITEMS 4
static char *menuItems[MENU_ITEMS];

void settingsMenu(SDL_Surface *surface, int selected, int value, int minValue, int maxValue, int i) {
    #define MIN(a, b) (a) < (b) ? (a) : (b)
    char *item = menuItems[i];
    int marginLeft = 32;
    int background = selected == SETTINGS_VOLUMN || selected == SETTINGS_SCREEN? SDL_MapRGB(surface->format, TRIAD_GRAY) : SDL_MapRGB(surface->format, TRIAD_WHITE);
    SDL_Surface *text;
    text = TTF_RenderUTF8_Blended(g_font.small, item, COLOR_LIGHT_TEXT);
    SDL_Surface *sIcon = i == SETTINGS_SCREEN ? g_gfx.brightness : (i == SETTINGS_VOLUMN ? g_gfx.volume : g_gfx.mute);
    SDL_Surface *sProgressEmpty = g_gfx.settings_bar_empty;
    SDL_Surface *sProgressBar = g_gfx.settings_bar_full;
    
    int row_width = text->w + marginLeft * 2;
    int max_width = MIN(row_width, 580);
    int row_cy = (ROW_HEIGHT / 2) - (text->h / 2);
    int screen_center = (SCREEN_HEIGHT / 2) - ((ROW_HEIGHT * ROW_COUNT) / 2);
    int iconMarginLeft = ICON_SIZE + 8;
    int cy = (sIcon->h / 2) - (sProgressEmpty->h / 2);
    int w = sProgressBar->w * ((float)(value - minValue) / (maxValue - minValue));
    int h = sProgressBar->h;

    if (i == selected) {
      text = TTF_RenderUTF8_Blended(g_font.small, item, COLOR_DARK_TEXT);
      SDL_FillRect(surface, &(SDL_Rect){0, screen_center + i * ROW_HEIGHT, max_width,  ROW_HEIGHT}, background);
      if (selected == SETTINGS_VOLUMN || selected == SETTINGS_SCREEN) {
        SDL_BlitSurface(sIcon, NULL, surface, &(SDL_Rect){marginLeft, screen_center + (i * ROW_HEIGHT) + row_cy});
        SDL_BlitSurface(sProgressEmpty, NULL, surface, &(SDL_Rect){marginLeft, screen_center + (i * ROW_HEIGHT) + row_cy});
        SDL_BlitSurface(sProgressBar, &(SDL_Rect){0, 0, w, h}, surface, &(SDL_Rect){marginLeft + iconMarginLeft, screen_center + (i * ROW_HEIGHT) + row_cy, w, h});
      } else {
        SDL_BlitSurface(text, &(SDL_Rect){0, 0, max_width, text->h}, surface, &(SDL_Rect){marginLeft, screen_center + (i * ROW_HEIGHT) + row_cy});
        SDL_FreeSurface(text);
      }
    } else if (i == SETTINGS_VOLUMN) {
      SDL_BlitSurface(sIcon, NULL, surface, &(SDL_Rect){marginLeft, screen_center + (i * ROW_HEIGHT) + row_cy});
      SDL_BlitSurface(sProgressEmpty, NULL, surface, &(SDL_Rect){marginLeft + iconMarginLeft, screen_center + (i * ROW_HEIGHT) + row_cy});
      SDL_BlitSurface(sProgressBar, &(SDL_Rect){0, 0, w, h}, surface, &(SDL_Rect){marginLeft + iconMarginLeft, screen_center + (i * ROW_HEIGHT) + row_cy, w, h});
    } else if (i == SETTINGS_SCREEN) {
      SDL_BlitSurface(sIcon, NULL, surface, &(SDL_Rect){marginLeft, screen_center + (i * ROW_HEIGHT) + row_cy});
      SDL_BlitSurface(sProgressEmpty, NULL, surface, &(SDL_Rect){marginLeft + iconMarginLeft, screen_center + (i * ROW_HEIGHT) + row_cy});
      SDL_BlitSurface(sProgressBar, &(SDL_Rect){0, 0, w, h}, surface, &(SDL_Rect){marginLeft + iconMarginLeft, screen_center + (i * ROW_HEIGHT) + row_cy, w, h});
    } else {
      SDL_BlitSurface(text, &(SDL_Rect){0, 0, max_width, text->h}, surface, &(SDL_Rect){marginLeft, screen_center + (i * ROW_HEIGHT) + row_cy});
      SDL_FreeSurface(text);
    }

    // if (i == selected) {
    //   // Selected rows
    //   text = TTF_RenderUTF8_Blended(g_font.small, item, COLOR_DARK_TEXT);
    //   SDL_FillRect(surface, &(SDL_Rect){0, screen_center + i * ROW_HEIGHT, max_width,  ROW_HEIGHT}, SDL_MapRGB(surface->format, selected==SETTINGS_VOLUMN || SETTINGS_SCREEN? TRIAD_GRAY:TRIAD_WHITE));

    //   if (selected == SETTINGS_VOLUMN || SETTINGS_SCREEN) {
    //     SDL_BlitSurface(sIcon, NULL, surface, &(SDL_Rect){marginLeft, screen_center + (i * ROW_HEIGHT) + row_cy});
    //     SDL_BlitSurface(sProgressEmpty, NULL, surface, &(SDL_Rect){marginLeft, screen_center + (i * ROW_HEIGHT) + row_cy});
    //     SDL_BlitSurface(sProgressBar, &(SDL_Rect){0, 0, w, h}, surface, &(SDL_Rect){marginLeft + iconMarginLeft, screen_center + (i * ROW_HEIGHT) + row_cy, w, h});
    //   } else {
    //     SDL_BlitSurface(text, &(SDL_Rect){0, 0, max_width, text->h}, surface, &(SDL_Rect){marginLeft, screen_center + (i * ROW_HEIGHT) + row_cy});
    //     SDL_FreeSurface(text);
    //   }
    // } else {
    //   if (selected == SETTINGS_VOLUMN || SETTINGS_SCREEN) {
    //     SDL_BlitSurface(sIcon, NULL, surface, &(SDL_Rect){marginLeft, screen_center + (i * ROW_HEIGHT) + row_cy});
    //     SDL_BlitSurface(sProgressEmpty, NULL, surface, &(SDL_Rect){marginLeft + iconMarginLeft, screen_center + (i * ROW_HEIGHT) + row_cy});
    //     SDL_BlitSurface(sProgressBar, &(SDL_Rect){0, 0, w, h}, surface, &(SDL_Rect){marginLeft + iconMarginLeft, screen_center + (i * ROW_HEIGHT) + row_cy, w, h});
    //   } else {
    //     SDL_BlitSurface(text, &(SDL_Rect){0, 0, max_width, text->h}, surface, &(SDL_Rect){marginLeft, screen_center + (i * ROW_HEIGHT) + row_cy});
    //     SDL_FreeSurface(text);
    //   }
    // }
}

void initSettings(SDL_Surface *surface) {
  menuItems[SETTINGS_SLEEP] = "Sleep device";
  menuItems[SETTINGS_POWER] = "Shut down";
  menuItems[SETTINGS_SCREEN] = "Screen Brightness";
  menuItems[SETTINGS_VOLUMN] = "Adjust volumn";
  // int background = SDL_MapRGB(surface->format, TRIAD_BLACK);
  InitSettings();
  int selected = 0;
  int dirty = 1;
  int quit = 0;
  int showSettings = 0;
  // Device settings
	int volBritValue = 0;
	int volBritMin = 0;
	int volBritMax = 0;
  ///


  while(!quit) {
    SDL_FillRect(surface, NULL, 0);
    unsigned long frameStart = SDL_GetTicks();
    Input_poll();

    if (Input_justPressed(BTN_UP)) {
        selected -= 1;
        if (selected<0) {
          selected += MENU_ITEMS;
        }
        dirty = 1;
      } else if (Input_justPressed(BTN_DOWN)) {
        selected += 1;
        if (selected>=MENU_ITEMS) {
          selected -= MENU_ITEMS;
        }
        dirty = 1;
      }

    if ((Input_justPressed(BTN_B) || Input_justReleased(BTN_MENU)) && !dirty) {
        quit = 1;
      } else if (Input_justPressed(BTN_A)) {
        switch(selected) {
          case SETTINGS_SLEEP:
          fauxSleep();
          quit = 1;
          break;
          case SETTINGS_POWER:
          powerOff();
          quit = 1;
          break;
        }
        //TODO: add status switch case to track options
    } else if (Input_justPressed(BTN_MINUS) || Input_justPressed(BTN_PLUS)) {
        switch(selected) {
        case SETTINGS_SCREEN:
        volBritValue = GetBrightness();
        volBritMax = MIN_BRIGHTNESS;
        volBritMax = MAX_BRIGHTNESS;
        break;
        case SETTINGS_VOLUMN:
        volBritValue = GetVolume();
        volBritMin = MIN_VOLUME;
        volBritMax = MAX_VOLUME;
        break;
        }
    }

    // Dirty
    if(dirty) {
      for (int i = 0; i < MENU_ITEMS; i++) {
        settingsMenu(surface, selected, volBritValue, volBritMin, volBritMax, i);
	      // SDL_FillRect(surface, NULL, background);
      }
      button(surface, "A", "Select", 0, 1, SCREEN_WIDTH - PADDING_LR, 419);
      button(surface, "B", "Close", 1, 1, SCREEN_WIDTH - PADDING_LR - 120, 419);
      SDL_Flip(surface);
      dirty = 0;
    }

    // slow down to 60fps
		unsigned long frameDuration = SDL_GetTicks() - frameStart;
		if (frameDuration<FRAME_DURATION) SDL_Delay(FRAME_DURATION-frameDuration);
  }

  SDL_FillRect(surface, NULL, 0);
  SDL_Flip(surface);
  SDL_Quit();
}