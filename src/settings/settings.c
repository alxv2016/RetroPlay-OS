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

void settingsMenu(SDL_Surface *surface, int selected, int i) {
    #define MIN(a, b) (a) < (b) ? (a) : (b)
    char *item = menuItems[i];
    int margin_left = 32;

    SDL_Surface *text;
    text = TTF_RenderUTF8_Blended(g_font.small, item, COLOR_LIGHT_TEXT);
    int row_width = text->w + margin_left * 2;
    int max_width = MIN(row_width, 580);
    int row_cy = (ROW_HEIGHT / 2) - (text->h / 2);
    int screen_center = (SCREEN_HEIGHT / 2) - ((ROW_HEIGHT * ROW_COUNT) / 2);

    if (i == selected) {
      // Selected rows
      text = TTF_RenderUTF8_Blended(g_font.small, item, COLOR_DARK_TEXT);
      SDL_FillRect(surface,
                   &(SDL_Rect){0, screen_center + i * ROW_HEIGHT, max_width,
                               ROW_HEIGHT},
                   SDL_MapRGB(surface->format, TRIAD_WHITE));
      SDL_BlitSurface(
          text, &(SDL_Rect){0, 0, max_width, text->h}, surface,
          &(SDL_Rect){margin_left,
                      screen_center + (i * ROW_HEIGHT) + row_cy});
      SDL_FreeSurface(text);
    } else {
      SDL_BlitSurface(
          text, &(SDL_Rect){0, 0, max_width, text->h}, surface,
          &(SDL_Rect){margin_left,
                      screen_center + (i * ROW_HEIGHT) + row_cy});
      SDL_FreeSurface(text);
    }
}

void initSettings(SDL_Surface *surface) {
  menuItems[SETTINGS_SLEEP] = "Sleep device";
  menuItems[SETTINGS_POWER] = "Shut down";
  menuItems[SETTINGS_SCREEN] = "Screen Brightness";
  menuItems[SETTINGS_VOLUMN] = "Adjust volumn";
  // int background = SDL_MapRGB(surface->format, TRIAD_BLACK);

  int selected = 0;
  int dirty = 1;
  int quit = 0;
  int showSettings = 0;
  // Device settings
  int show_setting = 0; // 1=brightness,2=volume
	int setting_value = 0;
	int setting_min = 0;
	int setting_max = 0;
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
    }

    // Dirty
    if(dirty) {
      int btn_a_width = getButtonWidth("Select", "A");
      for (int i = 0; i < MENU_ITEMS; i++) {
        settingsMenu(surface, selected, i);
	      // SDL_FillRect(surface, NULL, background);
      }
      button(surface, "A", "Select", 0, 557, 419);
      button(surface, "B", "Cancel", 1, 557 - btn_a_width, 419);
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