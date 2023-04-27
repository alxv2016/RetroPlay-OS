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
#include "../common/settings.h"

///////////////////////////////////////
static int volMin = MIN_VOLUME;
static int volMax = MAX_VOLUME;
static int britMin = MIN_BRIGHTNESS;
static int britMax = MAX_BRIGHTNESS;

int main(int argc, char *argv[]) {
  rumble(OFF);
  menuSuperShortPulse();
  if (autoResume()) return 0;
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
  TTF_Init();
  SDL_ShowCursor(0);
  SDL_EnableKeyRepeat(300, 100);
  InitSettings();
  gfx.screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 16, SDL_HWSURFACE | SDL_DOUBLEBUF);
  if (Mix_OpenAudio(48000, 32784, 2, 4096) < 0) return 0;
  GFX_init();
  GFX_ready();
  Menu_init();
  Input_reset();

  int dirty = 1;
  int chargeStatus = isCharging();
  unsigned long chargeStartTime = SDL_GetTicks();
  unsigned long powerStartTime = 0;
  unsigned long deviceSleepTime = SDL_GetTicks();
  // Volumne adjustment auto UI display timer
  unsigned long volumeAdjustTime = 0;
  // Settings menu current values
  int showSettingsMenu = 0;
  int settingItemSelected = 0;
  int currentSleepTime = getSleepTime() ? getSleepTime(): 1;
  int currentVolume = GetVolume();
  int currentBrightness = GetBrightness();

  while (!quit) {
    unsigned long frameStart = SDL_GetTicks();
    unsigned long currentTime = SDL_GetTicks();
    int oldSleepTime = currentSleepTime;
    int oldVolume = currentVolume;
    int oldBrightness = currentBrightness;
    int selected = top->selected;
    int total = top->entries->count;
    Input_poll();

    if (showSettingsMenu) {
      if ((Input_justPressed(BTN_B) || Input_justReleased(BTN_MENU))) {
        showSettingsMenu = 0;
        dirty = 1;
      } else if (Input_justPressed(BTN_A)) {
        switch (settingItemSelected) {
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
        currentVolume = GetVolume();
      } else if (Input_justRepeated(BTN_LEFT)) {
        switch (settingItemSelected) {
        case SETTINGS_SCREEN:
          currentBrightness = GetBrightness();
          if (currentBrightness > britMin)
            SetBrightness(--currentBrightness);
          if (currentBrightness == britMin)
            SetBrightness(britMin);
          break;
        case SETTINGS_VOLUMN:
          currentVolume = GetVolume();
          if (currentVolume > volMin)
            SetVolume(--currentVolume);
          if (currentVolume == volMin)
            SetMute(1);
          break;
        case SETTINGS_SLEEPTIME:
          currentSleepTime = getSleepTime();
          if (currentSleepTime > 0) {
            setSleepTime(--currentSleepTime);
            currentSleepTime = getSleepTime();
          }
          dirty = 1;
          break;
        }
      } else if (Input_justRepeated(BTN_RIGHT)) {
        switch (settingItemSelected) {
        case SETTINGS_SCREEN:
          currentBrightness = GetBrightness();
          if (currentBrightness < britMax)
            SetBrightness(++currentBrightness);
          break;
        case SETTINGS_VOLUMN:
          currentVolume = GetVolume();
          if (currentVolume < volMax)
            SetVolume(++currentVolume);
          if (currentVolume > volMin)
            SetMute(0);
          break;
        case SETTINGS_SLEEPTIME:
          currentSleepTime = getSleepTime();
          if (currentSleepTime < TIMER_ITEMS - 1) {
            setSleepTime(++currentSleepTime);
            currentSleepTime = getSleepTime();
          }
          dirty = 1;
          break;
        }
      }

      if (Input_justPressed(BTN_UP)) {
        settingItemSelected -= 1;
        if (settingItemSelected < 0) {
          settingItemSelected += MENU_ITEMS;
        }
        dirty = 1;
      } else if (Input_justPressed(BTN_DOWN)) {
        settingItemSelected += 1;
        if (settingItemSelected >= MENU_ITEMS) {
          settingItemSelected -= MENU_ITEMS;
        }
        dirty = 1;
      }

    } else if (Input_justReleased(BTN_MENU)) {
      showSettingsMenu = 1;
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

    // Device Charge, Sleep, Power logic
    if (dirty || currentTime - chargeStartTime >= CHARGE_DELAY) {
      int nowcharging = isCharging();
      if (chargeStatus != nowcharging) {
        chargeStatus = nowcharging;
        dirty = 1;
      }
      chargeStartTime = currentTime;
    }
    if (powerStartTime && currentTime - powerStartTime >= 1000) powerOff();
    if (Input_justPressed(BTN_POWER)) powerStartTime = currentTime;
    if (Input_anyPressed()) deviceSleepTime = currentTime;
    if (currentTime - deviceSleepTime >= SLEEP_DELAY && preventAutosleep()) deviceSleepTime = currentTime;
    if (currentTime - deviceSleepTime >= SLEEP_DELAY || Input_justReleased(BTN_POWER)) {
      fauxSleep();
      deviceSleepTime = SDL_GetTicks();
      powerStartTime = 0;
      dirty = 1;
    } 

    // dirty list (not including settings/battery)
    int was_dirty = dirty;

    if (Input_isPressed(BTN_MINUS) || Input_isPressed(BTN_PLUS)) {
      currentVolume = GetVolume();
    }

    if (oldVolume != currentVolume || oldBrightness != currentBrightness || oldSleepTime != currentSleepTime) {
      dirty = 1;
      volumeAdjustTime = SDL_GetTicks();
    }
    else if (volumeAdjustTime > 0 && SDL_GetTicks() - volumeAdjustTime > 500) {
      dirty = 1;
      volumeAdjustTime = 0;
    }

    if (dirty) {
      SDL_FillRect(gfx.screen, NULL, 0);
      if (showSettingsMenu) {
        initSettings(gfx.screen, settingItemSelected, currentVolume, currentBrightness, currentSleepTime);
      } else {
        if (total > 0) {
          int selected_row = top->selected - top->start;
          for (int i = top->start, j = 0; i < top->end; i++, j++) {
            Entry *entry = top->entries->items[i];
            listMenu(gfx.screen, entry->path, top->consoleDir, entry->emuTag, entry->name, entry->unique, j, selected_row);
          }
        } else {
          emptyState(gfx.screen, H2, CAPTION, "Couldn't find any games.", "Load some games to start playing.");
        }
      }

      // if (can_resume && !show_version) {
      //   if (strlen("X") > 1)
      //     button(gfx.screen, "X", "Resume", 0, 20, 419);
      //   else
      //     button(gfx.screen, "X", "Resume", 0, 557, 419);
      // }

      if (showSettingsMenu) {
        primaryBTN(gfx.screen, "A", "Select", 1, SCREEN_WIDTH - SPACING_LG,
               419);
        secondaryBTN(gfx.screen, "B", "Close", 1,
               SCREEN_WIDTH - SPACING_LG - 120, 419);
      } else {
        if (total == 0 && stack->count > 1) {
          primaryBTN(gfx.screen, "B", "Back", 1, SCREEN_WIDTH - SPACING_LG,
                 419);
        } else if (total > 0 && stack->count > 1) {
          primaryBTN(gfx.screen, "A", "Play", 1, SCREEN_WIDTH - SPACING_LG,
                 419);
          secondaryBTN(gfx.screen, "B", "Back", 1,
                 SCREEN_WIDTH - SPACING_LG - 101, 419);
        } else {
          primaryBTN(gfx.screen, "A", "Select", 1, SCREEN_WIDTH - SPACING_LG,
                 419);
        }
      }

      batteryStatus(gfx.screen, SCREEN_WIDTH - SPACING_LG, 12);

      if (volumeAdjustTime && !showSettingsMenu) {
        volumeControl(gfx.screen, SPACING_LG, 419, currentVolume, volMin, volMax);
      }
      SDL_Flip(gfx.screen);
      dirty = 0;
    }
    // slow down to 60fps
    GFX_sync(frameStart);
  }

  SDL_FillRect(gfx.screen, NULL, 0);
  SDL_Flip(gfx.screen);

  Menu_quit();
  GFX_quit();
  SDL_Quit();
  freeSound();
  QuitSettings();
}