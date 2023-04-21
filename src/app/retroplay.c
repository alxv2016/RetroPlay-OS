#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <msettings.h>

#include "../common/defines.h"
#include "../common/keycontext.h"

#include "../common/api.h"
#include "../common/controls.h"
#include "../common/interface.h"
#include "../common/powerops.h"
#include "../common/rumble.h"
#include "../settings/settings.h"

///////////////////////////////////////
GFX g_gfx;
int adjustVolumnBrit = 0;

int main(int argc, char *argv[]) {
  rumble(OFF);
  if (autoResume())
    return 0; // nothing to do
  menuSuperShortPulse();
  SDL_Init(SDL_INIT_VIDEO);
  TTF_Init();

  SDL_ShowCursor(0);
  SDL_EnableKeyRepeat(300, 100);

  InitSettings();
  g_gfx.screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 16,
                                  SDL_HWSURFACE | SDL_DOUBLEBUF);

  GFX_init();
  GFX_ready();
  Menu_init();
  Input_reset();

  int dirty = 1;
  int was_charging = isCharging();
  unsigned long charge_start = SDL_GetTicks();
  int showSettings = 0;
  int setting_value = 0;
  int setting_min = 0;
  int setting_max = 0;
  unsigned long setting_start = 0;
  unsigned long cancel_start = SDL_GetTicks();
  unsigned long power_start = 0;

  while (!quit) {
    unsigned long frameStart = SDL_GetTicks();
    Input_poll();
    int selected = top->selected;
    int total = top->entries->count;

    if (Input_justReleased(BTN_MENU) && !adjustVolumnBrit) {
      showSettings = 1;
      dirty = 1;
    }

    if (total > 0) {
      if (Input_justRepeated(BTN_UP)) {
        selected -= 1;
        if (selected < 0) {
          selected = total - 1;
          int start = total - ROW_COUNT;
          top->start = (start < 0) ? 0 : start;
          top->end = total;
        } else if (selected < top->start) {
          top->start -= 1;
          top->end -= 1;
        }
      } else if (Input_justRepeated(BTN_DOWN)) {
        selected += 1;
        if (selected >= total) {
          selected = 0;
          top->start = 0;
          top->end = (total < ROW_COUNT) ? total : ROW_COUNT;
        } else if (selected >= top->end) {
          top->start += 1;
          top->end += 1;
        }
      }
      if (Input_justRepeated(BTN_LEFT)) {
        // Scan games alphabetically
        Entry *entry = top->entries->items[selected];
        int i = entry->alpha - 1;
        if (i >= 0) {
          selected = top->alphas->items[i];
          if (total > ROW_COUNT) {
            top->start = selected;
            top->end = top->start + ROW_COUNT;
            if (top->end > total)
              top->end = total;
            top->start = top->end - ROW_COUNT;
          }
        }
      } else if (Input_justRepeated(BTN_RIGHT)) {
        Entry *entry = top->entries->items[selected];
        int i = entry->alpha + 1;
        if (i < top->alphas->count) {
          selected = top->alphas->items[i];
          if (total > ROW_COUNT) {
            top->start = selected;
            top->end = top->start + ROW_COUNT;
            if (top->end > total)
              top->end = total;
            top->start = top->end - ROW_COUNT;
          }
        }
      }
    }

    if (selected != top->selected) {
      top->selected = selected;
      dirty = 1;
    }
    /*   */
    if (dirty && total > 0)
      readyResume(top->entries->items[top->selected]);

    if (total > 0 && Input_justReleased(BTN_X)) {
      if (can_resume) {
        should_resume = 1;
        Entry_open(top->entries->items[top->selected]);
        dirty = 1;
      }
    } else if (total > 0 && Input_justPressed(BTN_A)) {
      Entry_open(top->entries->items[top->selected]);
      total = top->entries->count;
      dirty = 1;

      if (total > 0)
        readyResume(top->entries->items[top->selected]);
    } else if (Input_justPressed(BTN_B) && stack->count > 1) {
      closeDirectory();
      total = top->entries->count;
      dirty = 1;
      // can_resume = 0;
      if (total > 0)
        readyResume(top->entries->items[top->selected]);
    }

    unsigned long now = SDL_GetTicks();
    if (Input_anyPressed())
      cancel_start = now;

    if (dirty || now - charge_start >= CHARGE_DELAY) {
      int is_charging = isCharging();
      if (was_charging != is_charging) {
        was_charging = is_charging;
        dirty = 1;
      }
      charge_start = now;
    }

    if (power_start && now - power_start >= 1000) {
      powerOff();
      // return 0; // we should never reach this point
    }
    if (Input_justPressed(BTN_POWER)) {
      power_start = now;
    }

    if (now - cancel_start >= SLEEP_DELAY && preventAutosleep())
      cancel_start = now;

    if (now - cancel_start >= SLEEP_DELAY ||
        Input_justReleased(BTN_POWER)) // || Input_justPressed(BTN_MENU))
    {
      fauxSleep();
      cancel_start = SDL_GetTicks();
      power_start = 0;
      dirty = 1;
    }

    int was_dirty = dirty; // dirty list (not including settings/battery)

    int old_setting = adjustVolumnBrit;
    int old_value = setting_value;
    adjustVolumnBrit = 0;

    if (Input_isPressed(BTN_START) && (Input_isPressed(BTN_MINUS) || Input_isPressed(BTN_PLUS))) {
      adjustVolumnBrit = 1;
      setting_value = GetBrightness();
      setting_min = MIN_BRIGHTNESS;
      setting_max = MAX_BRIGHTNESS;
    }
    // Changed to start instead of menu to activate brightness
    else if (Input_isPressed(BTN_START) && old_setting == 1) {
      adjustVolumnBrit = 1;
      setting_value = GetBrightness();
      setting_min = MIN_BRIGHTNESS;
      setting_max = MAX_BRIGHTNESS;
    } else if (Input_isPressed(BTN_MINUS) || Input_isPressed(BTN_PLUS)) {
      adjustVolumnBrit = 2;
      setting_value = GetVolume();
      setting_min = MIN_VOLUME;
      setting_max = MAX_VOLUME;
    }

    if (old_setting && !adjustVolumnBrit)
      setting_start = SDL_GetTicks();

    if (old_value != setting_value)
      dirty = 1;
    else if (!old_setting && adjustVolumnBrit)
      dirty = 1;
    else if (setting_start > 0 && SDL_GetTicks() - setting_start > 500) {
      dirty = 1;
      setting_start = 0;
    }

    if (dirty) {
      SDL_FillRect(g_gfx.screen, NULL, 0);
      if (showSettings) {
        initSettings(g_gfx.screen);
      } else {
        if (total > 0) {
          int selected_row = top->selected - top->start;
          for (int i = top->start, j = 0; i < top->end; i++, j++) {
            Entry *entry = top->entries->items[i];
            listMenu(g_gfx.screen, entry->name, entry->path, entry->unique, j,
            selected_row);
          }
        } else {
          paragraph(g_gfx.screen, "No games detected,\n load some games.", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        }
      }

      // if (can_resume && !show_version) {
      //   if (strlen("X") > 1)
      //     button(g_gfx.screen, "X", "Resume", 0, 20, 419);
      //   else
      //     button(g_gfx.screen, "X", "Resume", 0, 557, 419);
      // }

      if (total == 0 && stack->count > 1) {
        button(g_gfx.screen, "B", "Back", 0, 1, SCREEN_WIDTH - PADDING_LR, 419);
      } else if (total > 0 && stack->count > 1) {
        button(g_gfx.screen, "A", "Play", 0, 1, SCREEN_WIDTH - PADDING_LR, 419);
        button(g_gfx.screen, "B", "Back", 1, 1, SCREEN_WIDTH - PADDING_LR - 101, 419);
      } else {
        button(g_gfx.screen, "A", "Select", 0, 1, SCREEN_WIDTH - PADDING_LR, 419);
      }

      batteryStatus(g_gfx.screen, 576, 12);

      if (adjustVolumnBrit) {
        volumnBrightness(g_gfx.screen, PADDING_LR, 419,
                        adjustVolumnBrit == VOLUME_ICON
                            ? BRIGHTNESS_ICON
                            : (setting_value > BRIGHTNESS_ICON
                                    ? VOLUME_ICON
                                    : VOLUME_MUTE_ICON),
                        setting_value, setting_min, setting_max);
      }
    }

    if (dirty) {
      SDL_Flip(g_gfx.screen);
      dirty = 0;
    }
    // slow down to 60fps
    GFX_sync(frameStart);
  }

  SDL_FillRect(g_gfx.screen, NULL, 0);
  SDL_Flip(g_gfx.screen);

  Menu_quit();
  GFX_quit();
  SDL_Quit();
  QuitSettings();
}