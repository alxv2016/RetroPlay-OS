#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
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
int utilIcon = VOLUME_ICON;
static int volMin = MIN_VOLUME;
static int volMax = MAX_VOLUME;
static int britMin = MIN_BRIGHTNESS;
static int britMax = MAX_BRIGHTNESS;

int main(int argc, char *argv[]) {
  rumble(OFF);
  menuSuperShortPulse();
  if (autoResume()) { return 0; }
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
  TTF_Init();

  SDL_ShowCursor(0);
  SDL_EnableKeyRepeat(300, 100);

  InitSettings();
  g_gfx.screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 16, SDL_HWSURFACE | SDL_DOUBLEBUF);
  Mix_OpenAudio(48000, 32784, 1, 4096);
  
  SDL_Surface *settingSurface;
  GFX_init();
  GFX_ready();
  Menu_init();
  Input_reset();

  int dirty = 1;
  int was_charging = isCharging();
  unsigned long charge_start = SDL_GetTicks();
  int showSettings = 0;
  int volValue = GetVolume();
  int britValue = GetBrightness();
  int settingSelected = 0;
  int timerSelected = getSleepTime() ? getSleepTime(): 1;
  // int setting_value = 0;
  // int setting_min = 0;
  // int setting_max = 0;
  unsigned long settingStart = 0;
  unsigned long cancelSettingStart = SDL_GetTicks();

  unsigned long power_start = 0;

  while (!quit) {
    unsigned long frameStart = SDL_GetTicks();
    Input_poll();
    int oldTimerValue = timerSelected;
    int oldVolValue = volValue;
    int oldBritValue = britValue;
    int oldUtilIcon = utilIcon;
    int selected = top->selected;
    int total = top->entries->count;

    if (showSettings) {
      if ((Input_justPressed(BTN_B) || Input_justReleased(BTN_MENU))) {
        showSettings = 0;
        dirty = 1;
      } else if (Input_justPressed(BTN_A)) {
        switch (settingSelected) {
        case SETTINGS_SLEEP:
          fauxSleep();
          quit = 1;
          break;
        case SETTINGS_POWER:
          powerOff();
          quit = 1;
          break;
        }
        // TODO: add status switch case to track options
      } else if (Input_justPressed(BTN_MINUS) || Input_justPressed(BTN_PLUS)) {
        playClick();
        volValue = GetVolume();
      } else if (Input_justRepeated(BTN_LEFT)) {
        switch (settingSelected) {
        case SETTINGS_SCREEN:
          britValue = GetBrightness();
          if (britValue > britMin)
            SetBrightness(--britValue);
          if (britValue == britMin)
            SetBrightness(britMin);
          break;
        case SETTINGS_VOLUMN:
          volValue = GetVolume();
          if (volValue > volMin)
            SetVolume(--volValue);
          if (volValue == volMin)
            SetMute(1);
          break;
        case SETTINGS_SLEEPTIME:
          timerSelected = getSleepTime();
          if (timerSelected > 0) {
            setSleepTime(--timerSelected);
            timerSelected = getSleepTime();
          }
          dirty = 1;
          break;
        }
      } else if (Input_justRepeated(BTN_RIGHT)) {
        switch (settingSelected) {
        case SETTINGS_SCREEN:
          britValue = GetBrightness();
          if (britValue < britMax)
            SetBrightness(++britValue);
          break;
        case SETTINGS_VOLUMN:
          volValue = GetVolume();
          if (volValue < volMax)
            SetVolume(++volValue);
          if (volValue > volMin)
            SetMute(0);
          break;
        case SETTINGS_SLEEPTIME:
          timerSelected = getSleepTime();
          if (timerSelected < TIMER_ITEMS - 1) {
            setSleepTime(++timerSelected);
            timerSelected = getSleepTime();
          }
          dirty = 1;
          break;
        }
      }

      if (Input_justPressed(BTN_UP)) {
        settingSelected -= 1;
        if (settingSelected < 0) {
          settingSelected += MENU_ITEMS;
        }
        dirty = 1;
      } else if (Input_justPressed(BTN_DOWN)) {
        settingSelected += 1;
        if (settingSelected >= MENU_ITEMS) {
          settingSelected -= MENU_ITEMS;
        }
        dirty = 1;
      }

    } else if (Input_justReleased(BTN_MENU)) {
      showSettings = 1;
      dirty = 1;
    } else {
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
    }

    unsigned long now = SDL_GetTicks();
    if (Input_anyPressed())
      cancelSettingStart = now;

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

    if (now - cancelSettingStart >= SLEEP_DELAY && preventAutosleep())
      cancelSettingStart = now;

    if (now - cancelSettingStart >= SLEEP_DELAY ||
        Input_justReleased(BTN_POWER)) // || Input_justPressed(BTN_MENU))
    {
      fauxSleep();
      cancelSettingStart = SDL_GetTicks();
      power_start = 0;
      dirty = 1;
    }

    int was_dirty = dirty; // dirty list (not including settings/battery)

    if (Input_isPressed(BTN_MINUS) || Input_isPressed(BTN_PLUS)) {
      utilIcon = VOLUME_ICON;
      volValue = GetVolume();
    }

    if (oldUtilIcon && !utilIcon)
      settingStart = SDL_GetTicks();

    if (oldVolValue != volValue || oldBritValue != britValue || oldTimerValue != timerSelected)
      dirty = 1;
    else if (!oldUtilIcon && utilIcon)
      dirty = 1;
    else if (settingStart > 0 && SDL_GetTicks() - settingStart > 500) {
      dirty = 1;
      settingStart = 0;
    }

    if (dirty) {
      SDL_FillRect(g_gfx.screen, NULL, 0);
      if (showSettings) {
        initSettings(g_gfx.screen, settingSelected, volValue, britValue, timerSelected);
        // showSomeSettings(g_gfx.screen);
        // SDL_BlitSurface(nscreen, NULL, g_gfx.screen, NULL);
      } else {
        if (total > 0) {
          int selected_row = top->selected - top->start;
          for (int i = top->start, j = 0; i < top->end; i++, j++) {
            Entry *entry = top->entries->items[i];
            listMenu(g_gfx.screen, entry->name, entry->path, entry->unique, j,
                     selected_row);
          }
        } else {
          paragraph(g_gfx.screen, "No games detected,\n load some games.", 0, 0,
                    SCREEN_WIDTH, SCREEN_HEIGHT);
        }
      }

      // if (can_resume && !show_version) {
      //   if (strlen("X") > 1)
      //     button(g_gfx.screen, "X", "Resume", 0, 20, 419);
      //   else
      //     button(g_gfx.screen, "X", "Resume", 0, 557, 419);
      // }

      if (showSettings) {
        button(g_gfx.screen, "A", "Select", 0, 1, SCREEN_WIDTH - PADDING_LR,
               419);
        button(g_gfx.screen, "B", "Close", 1, 1,
               SCREEN_WIDTH - PADDING_LR - 120, 419);
      } else {
        if (total == 0 && stack->count > 1) {
          button(g_gfx.screen, "B", "Back", 0, 1, SCREEN_WIDTH - PADDING_LR,
                 419);
        } else if (total > 0 && stack->count > 1) {
          button(g_gfx.screen, "A", "Play", 0, 1, SCREEN_WIDTH - PADDING_LR,
                 419);
          button(g_gfx.screen, "B", "Back", 1, 1,
                 SCREEN_WIDTH - PADDING_LR - 101, 419);
        } else {
          button(g_gfx.screen, "A", "Select", 0, 1, SCREEN_WIDTH - PADDING_LR,
                 419);
        }
      }

      batteryStatus(g_gfx.screen, 576, 12);

      if (utilIcon) {
        volumnBrightness(
            g_gfx.screen, PADDING_LR, 419,
            utilIcon == VOLUME_ICON
                ? BRIGHTNESS_ICON
                : (volValue > BRIGHTNESS_ICON ? VOLUME_ICON : MUTE_ICON),
            volValue, volMin, volMax);
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