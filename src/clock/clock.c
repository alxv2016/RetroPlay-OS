#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "../common/defines.h"

/* CLOCK COMPONENT */
#define MAX_YEAR 2100
#define MIN_YEAR 1970
#define DEFAULT_YEAR 2022
#define MAX_DAY 31
#define MIN_DAY 1
#define MAX_MONTH 12
#define MIN_MONTH 1
#define MAX_SECOND 59
#define MAX_MINUTE MAX_SECOND
#define MAX_HOUR 23
#define FRAME_DURATION 17

SDL_Surface *screen, *digits;
int select_cursor = 0;
char tmp_str[64];
char final_long_string[512];
uint8_t string_tmp[2][512];
uint32_t date_selected = MIN_DAY, month_selected = MIN_DAY,
         year_selected = MIN_YEAR;
uint32_t hour_selected = 0, minute_selected = 0, seconds_selected = 0;
uint32_t february_days = 28;
uint32_t update_clock = 0;

/* I could probably make this smaller */
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
static int renderDigit(SDL_Surface *digits, int value, int x, int y) {
	int width = 22;
	int height = 32; 
  SDL_BlitSurface(digits, &(SDL_Rect){value * width, 0, width, height}, screen, &(SDL_Rect){x, y});
  return x + width;
}

void selector(int x, int y, int len) {
	int width = 22;
	int height = 32;
	int totalWidth = len * width;
	uint32_t white = SDL_MapRGB(screen->format, 255,255,255);
	SDL_FillRect(screen, &(SDL_Rect){x,y + height,totalWidth,4}, white);
}

static int renderDigits(SDL_Surface *digits, int value, int x, int y) {
	int newValue;
	if (value > 999) {
			newValue = value / 1000;
			value -= newValue * 1000;
			x = renderDigit(digits, newValue, x,y);
			
			newValue = value / 100;
			value -= newValue * 100;
			x = renderDigit(digits, newValue, x,y);
		}
		newValue = value / 10;
		value -= newValue * 10;
		x = renderDigit(digits, newValue, x,y);
		
		newValue = value;
		x = renderDigit(digits, newValue, x,y);
		return x;
}

int main(int argc, char *argv[]) {
  int quit = 0;
  SDL_Event event;
  SDL_Init(SDL_INIT_VIDEO);
  SDL_ShowCursor(0);

  screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 16,
                            SDL_HWSURFACE | SDL_DOUBLEBUF);
  digits = IMG_Load(RES_PATH "/digits.png"); // 20x32
  if (!screen) {
    return 0;
  }

  time_t t = time(NULL);
  struct tm tm = *localtime(&t);

  date_selected = tm.tm_mday;
  month_selected = tm.tm_mon + 1;
  year_selected = tm.tm_year + 1900;
  hour_selected = tm.tm_hour;
  minute_selected = tm.tm_min;
  seconds_selected = tm.tm_sec;

  FallbackToDefaultTime(&date_selected, &month_selected, &year_selected,
                        &hour_selected, &minute_selected, &seconds_selected);

  SDL_EnableKeyRepeat(500, 50);

  while (!quit) {
		unsigned long frame_start = SDL_GetTicks();
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_KEYUP:
        switch (event.key.keysym.sym) {
        /* SDLK_HOME for OD needs the Power button to be detected upon
         * release. */
        case SDLK_HOME:
          quit = 1;
          break;
        default:
          break;
        }
        break;
      case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {
        case SDLK_UP:
          switch (select_cursor) {
          case 0:
					  year_selected++;
            break;
          case 1:
            month_selected++;
            break;
          case 2:
            date_selected++;
            break;
          case 3:
            hour_selected++;
            break;
          case 4:
            minute_selected++;
            break;
          case 5:
            seconds_selected++;
            break;
          }
          break;
        case SDLK_DOWN:
          switch (select_cursor) {
          case 0:
						DecreaseTimeElement(&year_selected, MAX_YEAR);
            break;
          case 1:
            DecreaseMonthElement(&month_selected, MAX_MONTH);
            break;
          case 2:
       			DecreaseDateElement(month_selected, &date_selected, MAX_DAY);
            break;
          case 3:
            DecreaseTimeElement(&hour_selected, MAX_HOUR);
            break;
          case 4:
            DecreaseTimeElement(&minute_selected, MAX_MINUTE);
            break;
          case 5:
            DecreaseTimeElement(&seconds_selected, MAX_SECOND);
            break;
          }
          break;
        case SDLK_LEFT:
          select_cursor--;
          if (select_cursor < 0)
            select_cursor += 6;
          break;
        case SDLK_RIGHT:
          select_cursor++;
          if (select_cursor > 5)
            select_cursor -= 6;
          break;
        case SDLK_RETURN:
          quit = 1;
          update_clock = 1;
          break;
        case SDLK_LCTRL:
          quit = 1;
          break;
        default:
          break;
        }
        break;
      case SDL_QUIT:
        quit = 1;
        break;
      }
    }

    Check_leap_year();
    Dont_go_over_days();
    Dont_go_over_hour();
    FallbackToDefaultTime(&date_selected, &month_selected, &year_selected,
                          &hour_selected, &minute_selected, &seconds_selected);


    SDL_FillRect(screen, NULL, 0);

		// datetime

		int dw = 22;
		int dh = 32;
    // Width of a single digit's with * the total amount of digits including seperators and spaces
    int datetimeTotalWidth = dw * 19;
		int cx = (SCREEN_WIDTH / 2) - (datetimeTotalWidth / 2);
		int cy = (SCREEN_HEIGHT / 2) - (dh / 2);
		int x = cx;
		int y = cy;
		

		x = renderDigits(digits, year_selected,x,y);
    x = renderDigit(digits, 10, x,y);
		x = renderDigits(digits, month_selected, x,y);
    x = renderDigit(digits, 10, x,y);
		x = renderDigits(digits, date_selected, x,y);
    // space
    x += dw;
		x = renderDigits(digits, hour_selected, x,y);
    x = renderDigit(digits, 11, x,y);
		x = renderDigits(digits, minute_selected, x,y);
    x = renderDigit(digits, 11, x,y);
		x = renderDigits(digits, seconds_selected, x,y);

		// cursor
		x = cx;
    //Each digit's width + seperator 22*5 (Year)
    int yrWidth = dw*5;
    int dateWith = dw*3;
		if (select_cursor>0) {
			x += yrWidth;
			x += (select_cursor - 1) * dateWith;
		}
		selector(x,y, (select_cursor>0 ? 2 : 4));
    SDL_Flip(screen);
		// slow down to 60fps
		unsigned long frame_duration = SDL_GetTicks() - frame_start;
		if (frame_duration<FRAME_DURATION) SDL_Delay(FRAME_DURATION-frame_duration);
  }

  if (screen) {
    SDL_FreeSurface(screen);
    SDL_FreeSurface(digits);
    screen = NULL;
  }

  SDL_Quit();

  if (update_clock == 1) {
    snprintf(final_long_string, sizeof(final_long_string),
             "date -s '%d-%d-%d %d:%d:%d';hwclock --utc -w", year_selected,
             month_selected, date_selected, hour_selected, minute_selected,
             seconds_selected);
    execlp("/bin/sh", "/bin/sh", "-c", final_long_string, (char *)NULL);
  }

  return 0;
}
