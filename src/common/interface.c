#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

#include "defines.h"
#include "powerops.h"
#include "utils.h"

#include "interface.h"

/* USER INTERFACE AND COMPONENTS */

GFX g_gfx;
Font g_font;

void GFX_init(void) {
  TTF_Init();
  g_font.large = TTF_OpenFont(FONT_PATH, FONT_LARGE);
  g_font.medium = TTF_OpenFont(FONT_PATH, FONT_MEDIUM);
  g_font.small = TTF_OpenFont(FONT_PATH, FONT_SMALL);
  g_font.tiny = TTF_OpenFont(FONT_PATH, FONT_TINY);

  g_gfx.button = loadImage("btn.png");
  g_gfx.bg_white = loadImage("bg-white.png");
  g_gfx.corner_radius = loadImage("radius-black.png");
  g_gfx.settings_bar_full = loadImage("settings-bar-full.png");
  g_gfx.settings_bar_empty = loadImage("settings-bar-empty.png");
  g_gfx.brightness = loadImage("brightness.png");
  g_gfx.volume = loadImage("volume.png");
  g_gfx.mute = loadImage("mute.png");
  g_gfx.battery_charge = loadImage("battery-charge.png");
  g_gfx.battery_power = loadImage("battery-power.png");
  g_gfx.battery = loadImage("battery.png");
  g_gfx.battery_low_power = loadImage("battery-low-power.png");
  g_gfx.battery_low = loadImage("battery-low.png");
}

void GFX_clear(void) {
  SDL_FillRect(g_gfx.screen, NULL, 0);
  SDL_Flip(g_gfx.screen);
}

void GFX_ready(void) {
  g_gfx.screen = SDL_GetVideoSurface(); // :cold_sweat:
}

void GFX_sync(unsigned long frameStart ) {
  unsigned long frameDuration = SDL_GetTicks() - frameStart;
  if (frameDuration < FRAME_DURATION) {
      SDL_Delay(FRAME_DURATION - frameDuration);
  }
}

void GFX_quit(void) {
  SDL_FreeSurface(g_gfx.button);
  SDL_FreeSurface(g_gfx.bg_white);
  SDL_FreeSurface(g_gfx.corner_radius);
  SDL_FreeSurface(g_gfx.settings_bar_full);
  SDL_FreeSurface(g_gfx.settings_bar_empty);
  SDL_FreeSurface(g_gfx.brightness);
  SDL_FreeSurface(g_gfx.volume);
  SDL_FreeSurface(g_gfx.mute);
  SDL_FreeSurface(g_gfx.battery_charge);
  SDL_FreeSurface(g_gfx.battery_power);
  SDL_FreeSurface(g_gfx.battery);
  SDL_FreeSurface(g_gfx.battery_low_power);
  SDL_FreeSurface(g_gfx.battery_low);

  TTF_CloseFont(g_font.large);
  TTF_CloseFont(g_font.medium);
  TTF_CloseFont(g_font.small);
  TTF_CloseFont(g_font.tiny);

  if (g_gfx.screen)
    SDL_FreeSurface(g_gfx.screen);
  TTF_Quit();
}

/* COMPONENTS */

void window(SDL_Surface *surface, int x, int y, int width, int height) {
  int radius_w = (RADIUS / 2);
  int radius_h = (RADIUS / 2);
  // Window corner radius
  SDL_BlitSurface(g_gfx.corner_radius, &(SDL_Rect){0, 0, radius_w, radius_h},
                  surface, &(SDL_Rect){x, y});
  SDL_BlitSurface(g_gfx.corner_radius,
                  &(SDL_Rect){radius_w, 0, radius_w, radius_h}, surface,
                  &(SDL_Rect){x + width - radius_w, y});
  SDL_BlitSurface(g_gfx.corner_radius,
                  &(SDL_Rect){0, radius_h, radius_w, radius_h}, surface,
                  &(SDL_Rect){x, y + height - radius_h});
  SDL_BlitSurface(g_gfx.corner_radius,
                  &(SDL_Rect){radius_w, radius_h, radius_w, radius_h}, surface,
                  &(SDL_Rect){x + width - radius_w, y + height - radius_h});

  // Window fill
  SDL_FillRect(surface, &(SDL_Rect){x + radius_w, y, width - RADIUS, radius_h},
               SDL_MapRGB(surface->format, TRIAD_BLACK));
  SDL_FillRect(surface, &(SDL_Rect){x, y + radius_h, width, height - RADIUS},
               SDL_MapRGB(surface->format, TRIAD_BLACK));
  SDL_FillRect(surface,
               &(SDL_Rect){x + radius_w, y + height - radius_h, width - RADIUS,
                           radius_h},
               SDL_MapRGB(surface->format, TRIAD_BLACK));
}

// Menu list component
void listMenu(SDL_Surface *surface, char *name, char *path, char *unique, int row, int selected_row)
{
#define MIN(a, b) (a) < (b) ? (a) : (b)
	char *display_name = unique ? unique : name;
	trimSortingMeta(&display_name);
	int margin_left = 32;

	SDL_Surface *text;
	text = TTF_RenderUTF8_Blended(g_font.small, display_name, COLOR_LIGHT_TEXT);
	int row_width = text->w + margin_left * 2;
	int text_width = truncateText(g_font.small, display_name, display_name, SCREEN_WIDTH / 2, margin_left) + margin_left;
	int max_width = MIN(row_width, text_width);
	int row_cy = (ROW_HEIGHT / 2) - (text->h / 2);
	int screen_center = (SCREEN_HEIGHT / 2) - ((ROW_HEIGHT * ROW_COUNT) / 2);

	if (row == selected_row)
	{
		// Selected rows
		text = TTF_RenderUTF8_Blended(g_font.small, display_name, COLOR_DARK_TEXT);
		SDL_FillRect(surface, &(SDL_Rect){0, screen_center + row * ROW_HEIGHT, max_width, ROW_HEIGHT}, SDL_MapRGB(surface->format, TRIAD_WHITE));
		SDL_BlitSurface(text, &(SDL_Rect){0, 0, max_width, text->h}, surface, &(SDL_Rect){margin_left, screen_center + (row * ROW_HEIGHT) + row_cy});
		SDL_FreeSurface(text);
	}
	else
	{
		SDL_BlitSurface(text, &(SDL_Rect){0, 0, max_width, text->h}, surface, &(SDL_Rect){margin_left, screen_center + (row * ROW_HEIGHT) + row_cy});
		SDL_FreeSurface(text);
	}
}

// Battery
void batteryStatus(SDL_Surface *surface, int x, int y) {
  int charge = getInt(BATTERY_INFO);
  SDL_Surface *batPower =
      charge <= 50 ? g_gfx.battery_low_power : g_gfx.battery_power;
  SDL_Surface *batIcon = g_gfx.battery;
  SDL_Surface *batLabel;

  char percentStr[5];
  sprintf(percentStr, "%i%%", charge);

  batLabel = TTF_RenderUTF8_Blended(g_font.small, percentStr, COLOR_LIGHT_TEXT);
  int marginRight = batLabel->w + 8;
  int batLabelCY = (batIcon->h / 2) - (batLabel->h / 2);

  if (isCharging()) {
    // NOTE: Not sure how we can get battery percent during charging.
    batLabel =
        TTF_RenderUTF8_Blended(g_font.small, "Charging", COLOR_LIGHT_TEXT);
    marginRight = batLabel->w + 8;
    SDL_BlitSurface(g_gfx.battery_charge, NULL, surface, &(SDL_Rect){x, y});
    SDL_BlitSurface(batLabel, NULL, surface,
                    &(SDL_Rect){x - marginRight, y + batLabelCY});
    SDL_FreeSurface(batLabel);
  } else {
    int pwrWidth = 24;
    int pwrHeight = 12;
    int pwrAmount = pwrWidth * charge / 100;
    // 1 is 1px right-offset from the center of the battery image
    int cx = (batIcon->w / 2) - (batPower->w / 2) - 1;
    int cy = (batIcon->h / 2) - (batPower->h / 2);

    SDL_Rect rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = pwrAmount;
    rect.h = pwrHeight;

    SDL_BlitSurface(batIcon, NULL, surface, &(SDL_Rect){x, y});
    SDL_BlitSurface(batPower, &rect, surface, &(SDL_Rect){x + cx, y + cy});
    SDL_BlitSurface(batLabel, NULL, surface,
                    &(SDL_Rect){x - marginRight, y + batLabelCY});
    SDL_FreeSurface(batLabel);
  }
}

// Return computed button width
int getButtonWidth(char *blabel, char *bkey) {
  int margin = 8;
  int btnWidth = 0;
  int computedWidth;

  if (strlen(bkey) == 1) {
    btnWidth += BUTTON_SIZE;
  } else {
    btnWidth += BUTTON_SIZE;
    TTF_SizeUTF8(g_font.medium, bkey, &computedWidth, NULL);
    btnWidth += computedWidth;
  }
  btnWidth += margin;

  TTF_SizeUTF8(g_font.small, blabel, &computedWidth, NULL);
  btnWidth += computedWidth + margin;
  return btnWidth;
}

// Button
void button(SDL_Surface *surface, char *bkey, char *blabel, int x, int y) {
  SDL_Surface *btn = g_gfx.button;
  SDL_Surface *btnKey =
      TTF_RenderUTF8_Blended(g_font.medium, bkey, COLOR_DARK_TEXT);
  SDL_Surface *btnLabel =
      TTF_RenderUTF8_Blended(g_font.small, blabel, COLOR_LIGHT_TEXT);

  int margin = 8;
  int btnCX = (btn->w / 2) - (btnKey->w / 2);
  // Bump 2px up to visually center letter in btn
  int btnCY = (btn->h / 2) - (btnKey->h / 2) - 1;
  int btnLabelCY = (btn->h / 2) - (btnLabel->h / 2);
  int btnX = BUTTON_SIZE + margin;

  SDL_Rect rect;
  rect.x = 0;
  rect.y = 0;
  rect.w = BUTTON_SIZE;
  rect.h = BUTTON_SIZE;

  SDL_BlitSurface(btnLabel, NULL, surface, &(SDL_Rect){x, y + btnLabelCY});
  SDL_FreeSurface(btnLabel);

  SDL_BlitSurface(btn, &rect, surface, &(SDL_Rect){x - btnX, y});
  SDL_BlitSurface(btnKey, NULL, surface,
                  &(SDL_Rect){x + btnCX - btnX, y + btnCY});
  SDL_FreeSurface(btnKey);
}

// Volumn settings component
void volumnBrightness(SDL_Surface *surface, int x, int y, int icon, int value,
                      int minValue, int maxValue) {
  SDL_Surface *sIcon =
      icon == 0 ? g_gfx.brightness : (icon == 1 ? g_gfx.volume : g_gfx.mute);
  SDL_Surface *sProgressEmpty = g_gfx.settings_bar_empty;
  SDL_Surface *sProgressBar = g_gfx.settings_bar_full;

  int cy = (sIcon->h / 2) - (sProgressEmpty->h / 2);
  int w = sProgressBar->w * ((float)(value - minValue) / (maxValue - minValue));
  int h = sProgressBar->h;
  int marginLeft = ICON_SIZE + 8;

  window(surface, x, y, SCREEN_WIDTH / 2, ICON_SIZE * 2);

  SDL_BlitSurface(sIcon, NULL, surface, &(SDL_Rect){x, y});
  SDL_BlitSurface(sProgressEmpty, NULL, surface,
                  &(SDL_Rect){x + marginLeft, y + cy});
  SDL_BlitSurface(sProgressBar, &(SDL_Rect){0, 0, w, h}, surface,
                  &(SDL_Rect){x + marginLeft, y + cy, w, h});
}

// Pill button
void pillButton(SDL_Surface *surface, char *bkey, char *blabel, int x, int y) {
  SDL_Surface *btn = g_gfx.button;
  SDL_Surface *btnKey =
      TTF_RenderUTF8_Blended(g_font.medium, bkey, COLOR_DARK_TEXT);
  SDL_Surface *btnLabel =
      TTF_RenderUTF8_Blended(g_font.small, blabel, COLOR_LIGHT_TEXT);

  // Pill's left radius
  SDL_Rect rectL;
  rectL.x = 0;
  rectL.y = 0;
  rectL.w = BUTTON_SIZE / 2;
  rectL.h = BUTTON_SIZE;

  SDL_Rect rectFill;
  rectFill.x = x + BUTTON_SIZE / 2;
  rectFill.y = y;
  rectFill.w = btnKey->w;
  rectFill.h = BUTTON_SIZE;

  SDL_Rect rectR;
  rectR.x = BUTTON_SIZE / 2;
  rectR.y = 0;
  rectR.w = BUTTON_SIZE / 2;
  rectR.h = BUTTON_SIZE;

  int margin = 8;
  int btnCX = ((btn->w / 2) + (btnKey->w / 2)) - (btnKey->w / 2);
  // Bump 2px up to visually center letter in btn
  int btnCY = (btn->h / 2) - (btnKey->h / 2) - 2;
  int btnLabelCY = (btn->h / 2) - (btnLabel->h / 2);
  int btnX = btn->w / 2 + btnKey->w + btn->w / 2 + margin;

  SDL_BlitSurface(btn, &rectL, surface, &(SDL_Rect){x, y});
  // Pill's fill container
  SDL_FillRect(surface, &rectFill, SDL_MapRGB(btn->format, TRIAD_WHITE));
  SDL_BlitSurface(btn, &rectR, surface,
                  &(SDL_Rect){x + btn->w / 2 + btnKey->w, y});
  SDL_BlitSurface(btnKey, NULL, surface, &(SDL_Rect){x + btnCX, y + btnCY});
  SDL_FreeSurface(btnKey);

  SDL_BlitSurface(btnLabel, NULL, surface,
                  &(SDL_Rect){x + btnX, y + btnLabelCY});
  SDL_FreeSurface(btnLabel);
}

// nameScroller
static int scrollSelected = -1;
static int scrollTicks = 0;
static int scrollDelay = 30;
static int scrollOffset = 0;
int nameScroller(SDL_Surface *surface, char *path, char *name, char *unique,
                 int maxWidth, int row, int selected, int reset, int force) {
  // reset is used when changing directories (otherwise returning from the first
  // row to the first row above wouldn't reset the scroll)
  if (reset || selected != scrollSelected) {
    scrollTicks = 0;
    scrollOffset = 0;
    scrollSelected = selected;
  }

  scrollTicks += 1;
  if (scrollTicks < scrollDelay)
    return 0; // nothing to do yet
  scrollOffset += 1;

  SDL_Surface *text;

  char *displayName = unique ? unique : name;
  trimSortingMeta(&displayName);

  if (text->w <= maxWidth) {
    SDL_FreeSurface(text);
    return 0;
  }
  // prevent overscroll
  if (scrollOffset > text->w - maxWidth) {
    scrollOffset = text->w - maxWidth;
    if (!force) { // nothing to draw unless something outside of this function
                  // dirtied the screen
      SDL_FreeSurface(text);
      return 0;
    }
  }

  SDL_FillRect(surface,
               &(SDL_Rect){0, 0 + (row * ROW_HEIGHT), SCREEN_WIDTH, ROW_HEIGHT},
               SDL_MapRGB(surface->format, TRIAD_WHITE));
  text = TTF_RenderUTF8_Blended(g_font.medium, displayName, COLOR_DARK_TEXT);

  int centerY = (ROW_HEIGHT - text->h) / 2;
  SDL_BlitSurface(text, &(SDL_Rect){0, 0, maxWidth, text->h}, surface,
                  &(SDL_Rect){PADDING, 0 + (row * ROW_HEIGHT) + centerY});
  SDL_FreeSurface(text);
  return 1;
}

SDL_Surface *loadImage(char *path) {
  static char fullPath[256];
  sprintf(fullPath, "%s/%s", RES_PATH, path);
  SDL_Surface *image = IMG_Load(fullPath);
  if (!image)
    printf("IMG_Load: %s\n", IMG_GetError());
  return image;
}

SDL_Surface *renderText(char *text) {
  SDL_Color color = COLOR_LIGHT_TEXT;
  return TTF_RenderUTF8_Blended(g_font.medium, text, color);
}

void hintLabel(SDL_Surface *surface, char *htxt, int x, int y) {
  SDL_Surface *hint_text =
      TTF_RenderUTF8_Blended(g_font.small, htxt, COLOR_LIGHT_TEXT);
  SDL_BlitSurface(hint_text, NULL, surface, &(SDL_Rect){x, y});
  SDL_FreeSurface(hint_text);
}

void paragraph(SDL_Surface *surface, char *str, int x, int y, int width,
               int height) {
  SDL_Surface *text;
  char *rows[MAX_ROW];
  int row_count = 0;

  char *tmp;
  rows[row_count++] = str;
  while ((tmp = strchr(rows[row_count - 1], '\n')) != NULL) {
    if (row_count + 1 >= MAX_ROW)
      return;
    rows[row_count++] = tmp + 1;
  }

  int rendered_height = FONT_LINEHEIGHT * row_count;
  y += (height - rendered_height) / 2;

  char line[MAX_PATH];
  for (int i = 0; i < row_count; i++) {
    int len;
    if (i + 1 < row_count) {
      len = rows[i + 1] - rows[i] - 1;
      if (len)
        strncpy(line, rows[i], len);
      line[len] = '\0';
    } else {
      len = strlen(rows[i]);
      strcpy(line, rows[i]);
    }

    if (len) {
      text = TTF_RenderUTF8_Blended(g_font.medium, line, COLOR_WHITE);
      x += (width - text->w) / 2;
      SDL_BlitSurface(text, NULL, surface, &(SDL_Rect){x, y});
      SDL_FreeSurface(text);
    }
    y += FONT_LINEHEIGHT;
  }
}

void inlineText(SDL_Surface *surface, char *str, int x, int y, int dark) {
  SDL_Surface *text;
  SDL_Color font_color = dark ? COLOR_DARK_TEXT : COLOR_LIGHT_TEXT;
  text = TTF_RenderUTF8_Blended(g_font.medium, str, font_color);
  SDL_BlitSurface(text, NULL, surface, &(SDL_Rect){x, y});
  SDL_FreeSurface(text);
}

int truncateText(TTF_Font *font, const char *in_name, char *out_name,
                 int max_width, int padding) {
  int text_width;
  strcpy(out_name, in_name);
  TTF_SizeUTF8(font, out_name, &text_width, NULL);
  text_width += padding;

  while (text_width > max_width) {
    int len = strlen(out_name);
    strcpy(&out_name[len - 4], "...\0");
    TTF_SizeUTF8(font, out_name, &text_width, NULL);
    text_width += padding;
  }

  return text_width;
}