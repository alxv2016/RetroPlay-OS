#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <msettings.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "../common/common.h"
#include "../common/defines.h"
#include "../common/arry.h"

///////////////////////////////////////

#define dump(msg)                                                              \
  puts((msg));                                                                 \
  fflush(stdout);

///////////////////////////////////////
///////////////////////////////////////
///////////////////////////////////////
///////////////////////////////////////
///////////////////////////////////////
///////////////////////////////////////


///////////////////////////////////////

// static Directory *top;
// // DirectoryArray
// static Array *stack;
// // RecentArray
// static Array *recents;

// static int quit = 0;
// static int can_resume = 0;
// // set to 1 on BTN_R1esume but only if can_resume==1
// static int should_resume = 0; 
// static char slot_path[256];

// static int restore_depth = -1;
// static int restore_relative = -1;
// static int restore_selected = 0;
// static int restore_start = 0;
// static int restore_end = 0;

// int quit = 0;
// int can_resume = 0;
// // set to 1 on BTN_R1esume but only if can_resume==1
// int should_resume = 0; 
// char slot_path[256];

// int restore_depth = -1;
// int restore_relative = -1;
// int restore_selected = 0;
// int restore_start = 0;
// int restore_end = 0;


///////////////////////////////////////




///////////////////////////////////////

///////////////////////////////////////


///////////////////////////////////////


///////////////////////////////////////


///////////////////////////////////////

int main(int argc, char *argv[]) {
  if (autoResume())
    return 0; // nothing to do

  dump("MiniUI");
  putenv("SDL_HIDE_BATTERY=1");
  SDL_Init(SDL_INIT_VIDEO);
  TTF_Init();

  SDL_ShowCursor(0);
  SDL_EnableKeyRepeat(300, 100);

  InitSettings();
  SDL_Surface *screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 16,
                                         SDL_HWSURFACE | SDL_DOUBLEBUF);
  GFX_init();
  GFX_ready();
  SDL_Surface *overlay;

  overlay = SDL_CreateRGBSurface(SDL_SWSURFACE, SCREEN_WIDTH, SCREEN_HEIGHT, 16,
                                 0, 0, 0, 0);
  SDL_SetAlpha(overlay, SDL_SRCALPHA, 0x90);

  Menu_init();
  Input_reset();

  int dirty = 1;
  int was_charging = isCharging();
  unsigned long charge_start = SDL_GetTicks();
  int btn_a_width = GFX_getButtonWidth("Open", "A");
  int show_version = 0;
  int show_setting = 0; // 1=brightness,2=volume
  int setting_value = 0;
  int setting_min = 0;
  int setting_max = 0;
  unsigned long setting_start = 0;
  unsigned long cancel_start = SDL_GetTicks();
  unsigned long power_start = 0;
  while (!quit) {
    unsigned long frame_start = SDL_GetTicks();

    Input_poll();

    int selected = top->selected;
    int total = top->entries->count;

    if (show_version) {
      if (Input_justPressed(BTN_B) || Input_justReleased(BTN_MENU)) {
        show_version = 0;
        dirty = 1;
      }
    } else {
      if (Input_justPressed(BTN_Y) ||
          Input_justReleased(BTN_MENU) && !show_setting) {
        show_version = 1;
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
          selected -= ROW_COUNT;
          if (selected < 0) {
            selected = 0;
            top->start = 0;
            top->end = (total < ROW_COUNT) ? total : ROW_COUNT;
          } else if (selected < top->start) {
            top->start -= ROW_COUNT;
            if (top->start < 0)
              top->start = 0;
            top->end = top->start + ROW_COUNT;
          }
        } else if (Input_justRepeated(BTN_RIGHT)) {
          selected += ROW_COUNT;
          if (selected >= total) {
            selected = total - 1;
            int start = total - ROW_COUNT;
            top->start = (start < 0) ? 0 : start;
            top->end = total;
          } else if (selected >= top->end) {
            top->end += ROW_COUNT;
            if (top->end > total)
              top->end = total;
            top->start = top->end - ROW_COUNT;
          }
        }
      }

      if (!Input_isPressed(BTN_START) && !Input_isPressed(BTN_SELECT)) {
        if (Input_justRepeated(BTN_L1)) { // previous alpha
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
        } else if (Input_justRepeated(BTN_R1)) { // next alpha
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

    unsigned long now = SDL_GetTicks();
    if (Input_anyPressed())
      cancel_start = now;

#define CHARGE_DELAY 1000
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

    // SLEEP DELAY TODO put into settings allow users to set time
#define SLEEP_DELAY 600000 // 10 Minutes
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

    int old_setting = show_setting;
    int old_value = setting_value;
    show_setting = 0;
    if (Input_isPressed(BTN_START) && Input_isPressed(BTN_SELECT)) {
      // ???? What's the purpose of this combo button press
    } else if (Input_isPressed(BTN_MENU) &&
               (Input_isPressed(BTN_MINUS) || Input_isPressed(BTN_PLUS))) {
      show_setting = 1;
      setting_value = GetBrightness();
      setting_min = MIN_BRIGHTNESS;
      setting_max = MAX_BRIGHTNESS;
    }
    // Changed to start instead of menu to activate brightness
    else if (Input_isPressed(BTN_START) && old_setting == 1) {
      show_setting = 1;
      setting_value = GetBrightness();
      setting_min = MIN_BRIGHTNESS;
      setting_max = MAX_BRIGHTNESS;
    } else if (Input_isPressed(BTN_MINUS) || Input_isPressed(BTN_PLUS)) {
      show_setting = 2;
      setting_value = GetVolume();
      setting_min = MIN_VOLUME;
      setting_max = MAX_VOLUME;
    }

    if (old_setting && !show_setting)
      setting_start = SDL_GetTicks();

    if (old_value != setting_value)
      dirty = 1;
    else if (!old_setting && show_setting)
      dirty = 1;
    else if (setting_start > 0 && SDL_GetTicks() - setting_start > 500) {
      dirty = 1;
      setting_start = 0;
    }

    if (dirty) {
      SDL_FillRect(screen, NULL, 0);

      if (show_setting) {
        GFX_blitSettings(screen, 0, 0,
                         show_setting == VOLUME_ICON
                             ? BRIGHTNESS_ICON
                             : (setting_value > BRIGHTNESS_ICON
                                    ? VOLUME_ICON
                                    : VOLUME_MUTE_ICON),
                         setting_value, setting_min, setting_max);
      } else {
        GFX_blitBattery(screen, 576, 12);
      }

      if (show_version) {
        // if (!version)
        // {
        // 	char release[256];
        // 	getFile("./version.txt", release, 256);

        // 	char *tmp, *commit;
        // 	commit = strrchr(release, '\n');
        // 	commit[0] = '\0';
        // 	commit = strrchr(release, '\n') + 1;
        // 	tmp = strchr(release, '\n');
        // 	tmp[0] = '\0';

        // 	SDL_Surface *release_txt = GFX_getText("Release");
        // 	SDL_Surface *version_txt = GFX_getText(release);
        // 	SDL_Surface *commit_txt = GFX_getText("Commit");
        // 	SDL_Surface *hash_txt = GFX_getText(commit);

        // 	SDL_Surface *firmware_txt = GFX_getText("Firmware");
        // 	SDL_Surface *date_txt = GFX_getText(getenv("MIYOO_VERSION"));

        // 	int x = firmware_txt->w + 12;
        // 	int w = x + version_txt->w;
        // 	int h = 96 * 2;
        // 	version = SDL_CreateRGBSurface(0, w, h, 16, 0, 0, 0, 0);

        // 	SDL_BlitSurface(release_txt, NULL, version, &(SDL_Rect){0, 0});
        // 	SDL_BlitSurface(version_txt, NULL, version, &(SDL_Rect){x, 0});
        // 	SDL_BlitSurface(commit_txt, NULL, version, &(SDL_Rect){0, 48});
        // 	SDL_BlitSurface(hash_txt, NULL, version, &(SDL_Rect){x, 48});
        // 	SDL_BlitSurface(firmware_txt, NULL, version, &(SDL_Rect){0,
        // 144}); 	SDL_BlitSurface(date_txt, NULL, version, &(SDL_Rect){x, 144});

        // 	SDL_FreeSurface(release_txt);
        // 	SDL_FreeSurface(version_txt);
        // 	SDL_FreeSurface(commit_txt);
        // 	SDL_FreeSurface(hash_txt);
        // 	SDL_FreeSurface(firmware_txt);
        // 	SDL_FreeSurface(date_txt);
        // }
        // TODO: show console list, move this to roms folders
        SDL_FillRect(overlay, NULL, 0);
        // SDL_BlitSurface(screen, NULL, screen, &(SDL_Rect){(SCREEN_WIDTH -
        // version->w) / 2, (SCREEN_HEIGHT - version->h) / 2});
        GFX_blitButton(screen, "B", "Back", 557, 419);
      } else {
        if (total > 0) {
          int selected_row = top->selected - top->start;
          for (int i = top->start, j = 0; i < top->end; i++, j++) {
            Entry *entry = top->entries->items[i];
            GFX_blitMainMenu(screen, entry->name, entry->path, entry->unique, j,
                             selected_row);
          }
        } else {
          // TODO: show console list, move this to roms folders
          GFX_blitParagraph(screen, "Empty folder", 0, 0, SCREEN_WIDTH,
                            SCREEN_HEIGHT);
        }
      }

      if (can_resume && !show_version) {
        if (strlen("X") > 1)
          GFX_blitButton(screen, "X", "Resume", 20, 419);
        else
          GFX_blitButton(screen, "X", "Resume", 557 - btn_a_width, 419);
      }

      // if (show_version)
      // {
      // 	// GFX_blitButton(screen, "B", "Back", 557 - btn_a_width, 419);
      // }
      // else if (total == 0)
      // {
      // 	if (stack->count > 1)
      // 	{
      // 		GFX_blitButton(screen, "B", "Back", 557 - btn_a_width,
      // 419);
      // 	}
      // }
      if (!show_version) {
        if (total == 0 && stack->count > 1) {
          GFX_blitButton(screen, "B", "Back", 557 - btn_a_width, 419);
        } else {
          GFX_blitButton(screen, "A", "Open", 557, 419);
          if (stack->count > 1) {
            GFX_blitButton(screen, "B", "Back", 557 - btn_a_width, 419);
          }
        }
      }
    }

    // scroll long names
    // if (total>0) {
    // 	int selected_row = top->selected - top->start;
    // 	Entry* entry = top->entries->items[top->selected];
    // 	if (GFX_scrollMenu(screen, entry->name, entry->path, entry->unique,
    // selected_row, top->selected, was_dirty, dirty)) dirty = 1;
    // }

    if (dirty) {
      SDL_Flip(screen);
      dirty = 0;
    }
    // slow down to 60fps
    unsigned long frame_duration = SDL_GetTicks() - frame_start;
#define FRAME_DURATION 17
    if (frame_duration < FRAME_DURATION)
      SDL_Delay(FRAME_DURATION - frame_duration);
  }

  SDL_FillRect(screen, NULL, 0);
  SDL_Flip(screen);

  Menu_quit();
  GFX_quit();
  SDL_Quit();
  QuitSettings();
}