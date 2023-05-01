#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>
#include <msettings.h>

#include "../common/defines.h"
#include "../common/keycontext.h"

#include "../common/utils.h"
#include "../common/controls.h"
#include "../common/interface.h"
#include "../common/rumble.h"
#include "clock.h"
///////////////////////////////////////
uint32_t date_selected = MIN_DAY, month_selected = MIN_DAY, year_selected = MIN_YEAR;
uint32_t hour_selected = 0, minute_selected = 0, seconds_selected = 0;
uint32_t february_days = 28;

static void Dont_go_over_days() {

  if (year_selected > MAX_YEAR) {
    year_selected = MIN_YEAR;
  }

  if (year_selected < MIN_YEAR) {
    year_selected = MIN_YEAR;
  }

  if (month_selected > MAX_MONTH) {
    month_selected = MIN_MONTH;
  }

  switch (month_selected) {
  case 2:
    if (date_selected > february_days)
      date_selected = MIN_DAY;
    break;
  case 4:
  case 6:
  case 9:
  case 11:
    if (date_selected > 30)
      date_selected = MIN_DAY;
    break;
  default:
    if (date_selected > MAX_DAY)
      date_selected = MIN_DAY;
    break;
  }
}

static void Check_leap_year() {
  if (((year_selected % 4 == 0) && (year_selected % 100 != 0)) ||
      (year_selected % 400 == 0))
    february_days = 29;
  else
    february_days = 28;
}

static void Dont_go_over_hour() {
  if (hour_selected > MAX_HOUR) {
    hour_selected = 0;
  }

  if (minute_selected > MAX_MINUTE) {
    minute_selected = 0;
  }

  if (seconds_selected > MAX_SECOND) {
    seconds_selected = 0;
  }
}

static void DecreaseMonthElement(uint32_t *month_element, int max) {
  if (*month_element == MIN_DAY) {
    *month_element = max;
  } else {
    (*month_element)--;
  }
}

static void DecreaseDateElement(uint32_t month_element, uint32_t *date_element,
                                int max) {
  int max_days = MAX_DAY;
  switch (month_element) {
  case 2:
    max_days = february_days;
    break;
  case 4:
  case 6:
  case 9:
  case 11:
    max_days = 30;
    break;
  default:
    break;
  }
  if (*date_element == MIN_DAY) {
    *date_element = max_days;
  } else {
    (*date_element)--;
  }
}

static void DecreaseTimeElement(uint32_t *time_element, int max) {
  if (*time_element == 0) {
    *time_element = max;
  } else {
    (*time_element)--;
  }
}

static void
FallbackToDefaultTime(uint32_t *date_selected, uint32_t *month_selected,
                      uint32_t *year_selected, uint32_t *hour_selected,
                      uint32_t *minute_selected, uint32_t *seconds_selected) {
  if (*year_selected == 1970) {
    *year_selected = DEFAULT_YEAR;
  }
}

/* UI */
static int renderDigit(SDL_Surface *surface, SDL_Surface *digits, int value, int x, int y) {
  int width = 22;
  int height = 32;
  SDL_BlitSurface(digits, &(SDL_Rect){value * width, 0, width, height}, surface,
                  &(SDL_Rect){x, y});
  return x + width;
}

void selector(SDL_Surface *surface, int x, int y, int len) {
  int width = 22;
  int height = 32;
  int totalWidth = len * width;
  uint32_t accent = SDL_MapRGB(surface->format, 219, 255, 77);
  SDL_FillRect(surface, &(SDL_Rect){x, y + height, totalWidth, 4}, accent);
}

static int renderDigits(SDL_Surface *surface, SDL_Surface *digits, int value, int x, int y) {
  int newValue;
  if (value > 999) {
    newValue = value / 1000;
    value -= newValue * 1000;
    x = renderDigit(surface, digits, newValue, x, y);

    newValue = value / 100;
    value -= newValue * 100;
    x = renderDigit(surface, digits, newValue, x, y);
  }
  newValue = value / 10;
  value -= newValue * 10;
  x = renderDigit(surface, digits, newValue, x, y);

  newValue = value;
  x = renderDigit(surface, digits, newValue, x, y);
  return x;
}

int main(int argc, char *argv[]) {
  rumble(OFF);
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
  SDL_ShowCursor(0);
  SDL_EnableKeyRepeat(500, 50);
  InitSettings();
  gfx.screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 16, SDL_HWSURFACE | SDL_DOUBLEBUF);
  if (Mix_OpenAudio(48000, 32784, 2, 4096) < 0) return 0;
  GFX_init();
  GFX_ready();
  Input_reset();

  int dirty = 1;
  int quit = 0;
  int update_clock = 0;
  int select_cursor = 0;

  time_t t = time(NULL);
  struct tm tm = *localtime(&t);

  date_selected = tm.tm_mday;
  month_selected = tm.tm_mon + 1;
  year_selected = tm.tm_year + 1900;
  hour_selected = tm.tm_hour;
  minute_selected = tm.tm_min;
  seconds_selected = tm.tm_sec;

  FallbackToDefaultTime(&date_selected, &month_selected, &year_selected, &hour_selected, &minute_selected, &seconds_selected);

  while (!quit) {
		unsigned long frameStart = SDL_GetTicks();
    Input_poll();

    if (Input_justRepeated(BTN_UP)) {
      playArrowSound();
      dirty = 1;
      switch (select_cursor) {
      case CURSOR_YEAR:
        year_selected++;
        break;
      case CURSOR_MONTH:
        month_selected++;
        break;
      case CURSOR_DAY:
        date_selected++;
        break;
      case CURSOR_HOUR:
        hour_selected++;
        break;
      case CURSOR_MINUTE:
        minute_selected++;
        break;
      case CURSOR_SECOND:
        seconds_selected++;
        break;
      }
    } else if (Input_justRepeated(BTN_DOWN)) {
      playArrowSound();
      dirty = 1;
      switch (select_cursor) {
      case CURSOR_YEAR:
        DecreaseTimeElement(&year_selected, MAX_YEAR);
        break;
      case CURSOR_MONTH:
        DecreaseMonthElement(&month_selected, MAX_MONTH);
        break;
      case CURSOR_DAY:
        DecreaseDateElement(month_selected, &date_selected, MAX_DAY);
        break;
      case CURSOR_HOUR:
        DecreaseTimeElement(&hour_selected, MAX_HOUR);
        break;
      case CURSOR_MINUTE:
        DecreaseTimeElement(&minute_selected, MAX_MINUTE);
        break;
      case CURSOR_SECOND:
        DecreaseTimeElement(&seconds_selected, MAX_SECOND);
        break;
      }
    } else if (Input_justRepeated(BTN_LEFT)) {
      playArrowSound();
      dirty = 1;
      select_cursor--;
      if (select_cursor < 0)
        select_cursor += 6;
    } else if (Input_justRepeated(BTN_RIGHT)) {
      playArrowSound();
      dirty = 1;
      select_cursor++;
      if (select_cursor > 5)
        select_cursor -= 6;
    } else if (Input_justPressed(BTN_A)) {
      playClickSound();
      SDL_Delay(200);
      quit = 1;
      update_clock = 1;
      superShortPulse();
    } else if (Input_justPressed(BTN_B)) {
      playClickSound();
      SDL_Delay(200);
      quit = 1;
    }

    if (dirty) {
      Check_leap_year();
      Dont_go_over_days();
      Dont_go_over_hour();
      FallbackToDefaultTime(&date_selected, &month_selected, &year_selected, &hour_selected, &minute_selected, &seconds_selected);
      SDL_FillRect(gfx.screen, NULL, 0);
      // datetime
      int dw = 22;
      int dh = 32;
      // Width of a single digit's with * the total amount of digits including seperators and spaces
      int datetimeTotalWidth = dw * 19;
      int cx = (SCREEN_WIDTH / 2) - (datetimeTotalWidth / 2);
      int cy = (SCREEN_HEIGHT / 2) - (dh / 2);
      int x = cx;
      int y = cy;
      
      x = renderDigits(gfx.screen, gfx.digits, year_selected, x, y);
      x = renderDigit(gfx.screen, gfx.digits, 10, x, y);
      x = renderDigits(gfx.screen, gfx.digits, month_selected, x, y);
      x = renderDigit(gfx.screen, gfx.digits, 10, x, y);
      x = renderDigits(gfx.screen, gfx.digits, date_selected, x, y);
      // space
      x += dw;
      x = renderDigits(gfx.screen, gfx.digits, hour_selected, x, y);
      x = renderDigit(gfx.screen, gfx.digits, 11, x, y);
      x = renderDigits(gfx.screen, gfx.digits, minute_selected, x, y);
      x = renderDigit(gfx.screen, gfx.digits, 11, x, y);
      x = renderDigits(gfx.screen, gfx.digits, seconds_selected, x, y);
      // cursor
      x = cx;
      //Each digit's width + seperator 22*5 (Year)
      int yrWidth = dw*5;
      int dateWith = dw*3;
      if (select_cursor>0) {
        x += yrWidth;
        x += (select_cursor - 1) * dateWith;
      }
      selector(gfx.screen, x, y, (select_cursor > 0 ? 2 : 4));
      // Button ui
      paragraph(BODY, 0, "Setup system time for games that\nsupport ingame time events.", (SDL_Color){NEUTRAL_TEXT}, gfx.screen, &(SDL_Rect){0, gfx.digits->h + SPACING_XL, gfx.screen->w, gfx.screen->h});
      primaryBTN(gfx.screen, "A", "Apply", 1, SCREEN_WIDTH - SPACING_LG, SCREEN_HEIGHT - SPACING_LG);
      secondaryBTN(gfx.screen, "B", "Cancel", 1, SCREEN_WIDTH - SPACING_LG - 113, SCREEN_HEIGHT - SPACING_LG);

      SDL_Flip(gfx.screen);
      dirty = 0;
    }
    // slow down to 60fps
    GFX_sync(frameStart);
  }

  GFX_quit();
  SDL_Quit();
  freeSound();
  QuitSettings();

  if (update_clock) {
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "date -s '%d-%d-%d %d:%d:%d';hwclock --utc -w", year_selected, month_selected, date_selected, hour_selected, minute_selected, seconds_selected);
    system(cmd);
  }

  return 0;
}
