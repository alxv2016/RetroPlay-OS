#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

#include "defines.h"
#include "utils.h"

#include "interface.h"

/* USER INTERFACE AND COMPONENTS */

GFX g_gfx;
Font font;

void GFX_init(void) {
  TTF_Init();

  font.h1 = TTF_OpenFont(FONT_PATH, H1);
  font.h2 = TTF_OpenFont(FONT_PATH, H2);
  font.h3 = TTF_OpenFont(FONT_PATH, H3);
  font.body = TTF_OpenFont(FONT_PATH, BODY);
  font.caption = TTF_OpenFont(FONT_PATH, CAPTION);
  font.footnote = TTF_OpenFont(FONT_PATH, FOOTNOTE);

  g_gfx.button = loadImage("btn.png");
  g_gfx.button_outline = loadImage("btn-outline.png");
  g_gfx.corner_radius = loadImage("radius-black.png");
  g_gfx.settings_bar_full = loadImage("progress-full.png");
  g_gfx.settings_bar_empty = loadImage("progress-empty.png");
  g_gfx.brightness = loadImage("brightness.png");
  g_gfx.volume = loadImage("volume.png");
  g_gfx.mute = loadImage("mute.png");
  g_gfx.battery_charge = loadImage("battery-charge.png");
  g_gfx.battery_power = loadImage("battery-power.png");
  g_gfx.battery = loadImage("battery.png");
  g_gfx.battery_low_power = loadImage("battery-low-power.png");
  g_gfx.battery_low = loadImage("battery-low.png");
  g_gfx.power = loadImage("power.png");
  g_gfx.sleep = loadImage("sleep.png");
  g_gfx.recents = loadImage("recents.png");
  g_gfx.apps = loadImage("apps.png");
  g_gfx.sleep_timer = loadImage("sleep-timer.png");
  g_gfx.arcade = loadImage("arcade.png");
  g_gfx.nes = loadImage("nes.png");
  g_gfx.gameboy = loadImage("gameboy.png");
  g_gfx.gba = loadImage("gba.png");
  g_gfx.gbc = loadImage("gbc.png");
  g_gfx.sega = loadImage("sega.png");
  g_gfx.playstation = loadImage("playstation.png");
  g_gfx.snes = loadImage("snes.png");
  g_gfx.sys_arcade = loadImage("sys-arcade.png");
  g_gfx.sys_gb = loadImage("sys-gb.png");
  g_gfx.sys_gba = loadImage("sys-gba.png");
  g_gfx.sys_gbc = loadImage("sys-gbc.png");
  g_gfx.sys_gg = loadImage("sys-gg.png");
  g_gfx.sys_nes = loadImage("sys-nes.png");
  g_gfx.sys_playstation = loadImage("sys-playstation.png");
  g_gfx.sys_sega = loadImage("sys-sega.png");
  g_gfx.sys_snes = loadImage("sys-snes.png");
  g_gfx.empty_state = loadImage("empty-folder.png");
}

void GFX_clear(void) {
  SDL_FillRect(g_gfx.screen, NULL, 0);
  SDL_Flip(g_gfx.screen);
}

void GFX_ready(void) {
  g_gfx.screen = SDL_GetVideoSurface(); // :cold_sweat:
}

void GFX_sync(unsigned long frameStart) {
  unsigned long frameDuration = SDL_GetTicks() - frameStart;
  if (frameDuration < FRAME_DURATION) {
    SDL_Delay(FRAME_DURATION - frameDuration);
  }
}

void GFX_quit(void) {
  SDL_FreeSurface(g_gfx.button);
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
  SDL_FreeSurface(g_gfx.recents);
  SDL_FreeSurface(g_gfx.apps);
  SDL_FreeSurface(g_gfx.arcade);
  SDL_FreeSurface(g_gfx.nes);
  SDL_FreeSurface(g_gfx.gameboy);
  SDL_FreeSurface(g_gfx.gba);
  SDL_FreeSurface(g_gfx.gbc);
  SDL_FreeSurface(g_gfx.sega);
  SDL_FreeSurface(g_gfx.playstation);
  SDL_FreeSurface(g_gfx.snes);
  SDL_FreeSurface(g_gfx.sys_arcade);
  SDL_FreeSurface(g_gfx.sys_gb);
  SDL_FreeSurface(g_gfx.sys_gba);
  SDL_FreeSurface(g_gfx.sys_gbc);
  SDL_FreeSurface(g_gfx.sys_gg);
  SDL_FreeSurface(g_gfx.sys_nes);
  SDL_FreeSurface(g_gfx.sys_playstation);
  SDL_FreeSurface(g_gfx.sys_sega);
  SDL_FreeSurface(g_gfx.sys_snes);
  SDL_FreeSurface(g_gfx.empty_state);

  TTF_CloseFont(font.h1);
  TTF_CloseFont(font.h2);
  TTF_CloseFont(font.h3);
  TTF_CloseFont(font.body);
  TTF_CloseFont(font.caption);
  TTF_CloseFont(font.footnote);

  if (g_gfx.screen)
    SDL_FreeSurface(g_gfx.screen);
  TTF_Quit();
}

/* COMPONENTS */

static void listItem(SDL_Surface *surface, SDL_Surface *icon, int showIcon, char *displayName, int row, int selected_row) {
  #define MIN(min, max) (min) < (max) ? (min) : (max)
  int marginLeft = 32;
  int titleMarginLeft = showIcon? marginLeft + CONSOLE_ICON_SIZE + 18: marginLeft;
  int availableWidth = SCREEN_WIDTH - marginLeft * 2;
  int titleWidth = truncateText(font.h3, displayName, availableWidth, marginLeft * 2);
  SDL_Surface *title = TTF_RenderUTF8_Blended(font.h3, displayName, LIGHT_TEXT);
  int accent = SDL_MapRGB(surface->format, PRIMARY);
  int background = SDL_MapRGB(surface->format, GREY500);

  int rowWidth = showIcon? marginLeft + CONSOLE_ICON_SIZE + 18 + title->w + marginLeft: marginLeft + title->w + marginLeft;
  // if (!showIcon) rowWidth = marginLeft + title->w + marginLeft;
  int maxTitleWidth = MIN(rowWidth, titleWidth);
  int row_cy = (ROW_HEIGHT / 2) - (title->h / 2);
  int icon_cy = (ROW_HEIGHT / 2) - (48 / 2);
  int screen_center = (SCREEN_HEIGHT / 2) - ((ROW_HEIGHT * ROW_COUNT) / 2);

  if (row == selected_row) {
    SDL_FillRect(surface, &(SDL_Rect){0, screen_center + row * ROW_HEIGHT, rowWidth, ROW_HEIGHT}, background);
    SDL_FillRect(surface,&(SDL_Rect){0, screen_center + row * ROW_HEIGHT, 6, ROW_HEIGHT}, accent);
  }

  if (showIcon) {
    SDL_BlitSurface(icon, NULL, surface, &(SDL_Rect){marginLeft, screen_center + (row * ROW_HEIGHT) + icon_cy});
  }
    SDL_BlitSurface(title, &(SDL_Rect){0, 0, maxTitleWidth, title->h}, surface, &(SDL_Rect){titleMarginLeft, screen_center + (row * ROW_HEIGHT) + row_cy});
    SDL_FreeSurface(title);
}

// Menu list component
void listMenu(SDL_Surface *surface, char *path, int consoleDir, char *emuTag, char *name, char *unique, int row, int selected_row) {
  SDL_Surface *sysCover;
  char *display_name = unique ? unique : name;
  trimSortingMeta(&display_name);
  sysCover = g_gfx.sys_arcade;
  int w = sysCover->w;
  int x = SCREEN_WIDTH - (w + SPACING_LG);
  int cy = (SCREEN_HEIGHT / 2) - (sysCover->h / 2);
  // Display console icons on root directory
  if (!strcmp(emuTag, "FBA") && !consoleDir) {
    listItem(surface, g_gfx.arcade, 1, display_name, row, selected_row);
  } else if (!strcmp(emuTag, "FC") && !consoleDir) {
    listItem(surface, g_gfx.nes, 1, display_name, row, selected_row);
  } else if (!strcmp(emuTag, "GB") && !consoleDir) {
    listItem(surface, g_gfx.gameboy, 1, display_name, row, selected_row);
  } else if (!strcmp(emuTag, "GBA") && !consoleDir) {
    listItem(surface, g_gfx.gba, 1, display_name, row, selected_row);
  } else if (!strcmp(emuTag, "GBC") && !consoleDir) {
    listItem(surface,g_gfx.gbc, 1, display_name, row, selected_row);
  } else if (!strcmp(emuTag, "MD") && !consoleDir) {
    listItem(surface, g_gfx.sega, 1, display_name, row, selected_row);
  } else if (!strcmp(emuTag, "PS") && !consoleDir) {
    listItem(surface, g_gfx.playstation, 1, display_name, row, selected_row);
  } else if (!strcmp(emuTag, "SFC") && !consoleDir) {
    listItem(surface,g_gfx.snes, 1, display_name, row, selected_row);
  } else if (!strcmp(name, "Apps") && !consoleDir) {
    listItem(surface,g_gfx.apps, 1, display_name, row, selected_row);
  } else if (!strcmp(name, "Recently Played")) {
    listItem(surface,g_gfx.recents, 1, display_name, row, selected_row);
  } else {
    // Display system covers images within rom directory
    // if (!strcmp(emuTag, "FBA") && consoleDir) {
    //   SDL_BlitSurface(sysCover, NULL, surface, &(SDL_Rect){x, cy});
    // } else if (!strcmp(emuTag, "FC") && consoleDir) {
    //   sysCover = g_gfx.sys_nes;
    //   SDL_BlitSurface(sysCover, NULL, surface, &(SDL_Rect){x, cy});
    // } else if (!strcmp(emuTag, "GB") && consoleDir) {
    //   sysCover = g_gfx.sys_gb;
    //   SDL_BlitSurface(sysCover, NULL, surface, &(SDL_Rect){x, cy});
    // } else if (!strcmp(emuTag, "GBA") && consoleDir) {
    //   sysCover = g_gfx.sys_gba;
    //   SDL_BlitSurface(sysCover, NULL, surface, &(SDL_Rect){x, cy});
    // } else if (!strcmp(emuTag, "GBC") && consoleDir) {
    //   sysCover = g_gfx.sys_gbc;
    //   SDL_BlitSurface(sysCover, NULL, surface, &(SDL_Rect){x, cy});
    // } else if (!strcmp(emuTag, "MD") && consoleDir) {
    //   sysCover = g_gfx.sys_sega;
    //   SDL_BlitSurface(sysCover, NULL, surface, &(SDL_Rect){x, cy});
    // } else if (!strcmp(emuTag, "PS") && consoleDir) {
    //   sysCover = g_gfx.sys_playstation;
    //   SDL_BlitSurface(sysCover, NULL, surface, &(SDL_Rect){x, cy});
    // } else if (!strcmp(emuTag, "SFC") && consoleDir) {
    //   sysCover = g_gfx.sys_snes;
    //   SDL_BlitSurface(sysCover, NULL, surface, &(SDL_Rect){x, cy});
    // }
    // Just passing in any dummy icon for non consoles items
    listItem(surface,g_gfx.nes, 0, display_name, row, selected_row);
  }

}

// Battery
void batteryStatus(SDL_Surface *surface, int x, int y) {
  int charge = getInt(BATTERY_INFO);
  SDL_Surface *batPower = charge < 40 ? g_gfx.battery_low_power : g_gfx.battery_power;
  SDL_Surface *batIcon = g_gfx.battery;
  SDL_Surface *batLabel;

  char percentStr[5];
  sprintf(percentStr, "%i%%", charge);
  batLabel = TTF_RenderUTF8_Blended(font.body, percentStr, LIGHT_TEXT);
  
  int margin = 8;
  int batteryRightAlign = x - batIcon->w;
  int labelRightAlign = x - (batLabel->w + batIcon->w + margin);
  int batLabelCY = (batIcon->h / 2) - (batLabel->h / 2);

  if (isCharging()) {
    // NOTE: Not sure how we can get battery percent during charging.
    batLabel = TTF_RenderUTF8_Blended(font.body, "Charging", LIGHT_TEXT);
    labelRightAlign = x - (batLabel->w + batIcon->w + margin);
    SDL_BlitSurface(g_gfx.battery_charge, NULL, surface, &(SDL_Rect){batteryRightAlign, y});
    SDL_BlitSurface(batLabel, NULL, surface, &(SDL_Rect){labelRightAlign, y + batLabelCY});
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

    SDL_BlitSurface(batIcon, NULL, surface, &(SDL_Rect){batteryRightAlign, y});
    SDL_BlitSurface(batPower, &rect, surface, &(SDL_Rect){batteryRightAlign + cx, y + cy});
    SDL_BlitSurface(batLabel, NULL, surface, &(SDL_Rect){labelRightAlign, y + batLabelCY});
    SDL_FreeSurface(batLabel);
  }
}

// Return computed button width
// NOTE: this doesn't return accurate widths...not sure why
int getButtonWidth(char *blabel) {
  SDL_Surface* TextSurface;
  int margin = 6;
  int btnWidth = BUTTON_SIZE + margin;
  TextSurface = TTF_RenderUTF8_Solid(font.h3, blabel, LIGHT_TEXT);
  SDL_FreeSurface(TextSurface);
  btnWidth += TextSurface->w;
  return btnWidth;
}

// Button
void button(SDL_Surface *surface, char *bkey, char *blabel, int outline, int rightAlign, int x, int y) {
  SDL_Surface *btn = outline? g_gfx.button_outline : g_gfx.button;
  SDL_Surface *btnKey =
      TTF_RenderUTF8_Blended(font.body, bkey, outline?LIGHT_TEXT:DARK_TEXT);
  SDL_Surface *btnLabel =
      TTF_RenderUTF8_Blended(font.body, blabel, LIGHT_TEXT);

  int margin = 6;
  int btnCX = (btn->w / 2) - (btnKey->w / 2);
  // Bump 2px up to visually center letter in btn
  int btnCY = (btn->h / 2) - (btnKey->h / 2) - 1;
  int btnLabelCY = (btn->h / 2) - (btnLabel->h / 2);
  int btnX = BUTTON_SIZE + margin;
  int labelRightAlign = x - btnLabel->w;
  int btnRightAlign = x - (btnLabel->w + btnX);

  SDL_Rect rect;
  rect.x = 0;
  rect.y = 0;
  rect.w = BUTTON_SIZE;
  rect.h = BUTTON_SIZE;

  SDL_BlitSurface(btnLabel, NULL, surface, &(SDL_Rect){rightAlign? labelRightAlign : x, y + btnLabelCY});
  SDL_FreeSurface(btnLabel);
  SDL_BlitSurface(btn, &rect, surface, &(SDL_Rect){rightAlign? btnRightAlign : x - btnX, y});
  SDL_BlitSurface(btnKey, NULL, surface, &(SDL_Rect){rightAlign? btnRightAlign + btnCX :x + btnCX - btnX, y + btnCY});
  SDL_FreeSurface(btnKey);
}

// Volumn settings component
void volumnBrightness(SDL_Surface *surface, int x, int y, int icon, int value, int minValue, int maxValue) {
  SDL_Surface *displayIcon = icon == 0 ? g_gfx.brightness : (icon == 1 ? g_gfx.volume : g_gfx.mute);
  int marginLeft = ICON_SIZE + 8;
  int w = SCREEN_WIDTH / 2 - marginLeft;
  int h = 4;
  int pw = w * ((float)(value - minValue) / (maxValue - minValue));
  int cy = (displayIcon->h / 2) - (h / 2);
  int progress = SDL_MapRGB(surface->format, WHITE);
  int background = SDL_MapRGB(surface->format, GREY400);

  SDL_BlitSurface(displayIcon, NULL, surface, &(SDL_Rect){x, y});
  SDL_FillRect(surface, &(SDL_Rect){x + marginLeft, y + cy, w, h}, background);
  SDL_FillRect(surface, &(SDL_Rect){x + marginLeft, y + cy, pw, h}, progress);
}

// Pill button
void pillButton(SDL_Surface *surface, char *bkey, char *blabel, int x, int y) {
  SDL_Surface *btn = g_gfx.button;
  SDL_Surface *btnKey =
      TTF_RenderUTF8_Blended(font.body, bkey, DARK_TEXT);
  SDL_Surface *btnLabel =
      TTF_RenderUTF8_Blended(font.body, blabel, LIGHT_TEXT);

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

  int margin = 6;
  int btnCX = ((btn->w / 2) + (btnKey->w / 2)) - (btnKey->w / 2);
  // Bump 2px up to visually center letter in btn
  int btnCY = (btn->h / 2) - (btnKey->h / 2) - 2;
  int btnLabelCY = (btn->h / 2) - (btnLabel->h / 2);
  int btnX = btn->w / 2 + btnKey->w + btn->w / 2 + margin;

  SDL_BlitSurface(btn, &rectL, surface, &(SDL_Rect){x, y});
  // Pill's fill container
  SDL_FillRect(surface, &rectFill, SDL_MapRGB(btn->format, WHITE));
  SDL_BlitSurface(btn, &rectR, surface,
                  &(SDL_Rect){x + btn->w / 2 + btnKey->w, y});
  SDL_BlitSurface(btnKey, NULL, surface, &(SDL_Rect){x + btnCX, y + btnCY});
  SDL_FreeSurface(btnKey);

  SDL_BlitSurface(btnLabel, NULL, surface,
                  &(SDL_Rect){x + btnX, y + btnLabelCY});
  SDL_FreeSurface(btnLabel);
}

void emptyState(SDL_Surface *surface, TTF_Font *font, int lineHeight, char *msg) {
  SDL_Surface *emptyStateIcon = g_gfx.empty_state;
  int msgWidth;
  int msgHeight;
  TTF_SizeUTF8(font, msg, &msgWidth, &msgHeight);
  int cx = (SCREEN_WIDTH / 2) - (emptyStateIcon->w / 2);
  int cy = (SCREEN_HEIGHT / 2) - (emptyStateIcon->h / 2);

  SDL_BlitSurface(emptyStateIcon, NULL, surface, &(SDL_Rect){cx, cy - emptyStateIcon->h/2});
  paragraph(font, msg, lineHeight, surface, &(SDL_Rect){0, emptyStateIcon->h/2, surface->w, surface->h});
}

static void headingCopyCombo(SDL_Surface *surface, TTF_Font *heading, TTF_Font *body, char *headingCopy, char *bodyCopy) {
  int headingLineHeight = 40;
  int bodyLineHeight = 33;
  int margin = 24;
  int headingHeight;
  int bodyHeight;
  TTF_SizeUTF8(heading, headingCopy, NULL, &headingHeight);
  TTF_SizeUTF8(body, bodyCopy, NULL, &bodyHeight);
  paragraph(heading, headingCopy, headingLineHeight, surface, &(SDL_Rect){0, -(headingHeight + margin / 2), surface->w, surface->h});
  paragraph(body, bodyCopy, bodyLineHeight, surface, &(SDL_Rect){0, bodyHeight + (margin / 2), surface->w, surface->h});
}

void emptyState2(SDL_Surface *surface, TTF_Font *heading, TTF_Font *body, char *headingCopy, char *bodyCopy) {
  // SDL_Surface *emptyStateIcon = g_gfx.empty_state;
  // int msgWidth;
  // int msgHeight;
  // TTF_SizeUTF8(font, msg, &msgWidth, &msgHeight);
  // int cx = (SCREEN_WIDTH / 2) - (emptyStateIcon->w / 2);
  // int cy = (SCREEN_HEIGHT / 2) - (emptyStateIcon->h / 2);

  // SDL_BlitSurface(emptyStateIcon, NULL, surface, &(SDL_Rect){cx, cy - emptyStateIcon->h/2});
  headingCopyCombo(surface, heading, body, headingCopy, bodyCopy);
}

// nameScroller - Deprecated (opting for truncation instead)
// static int scrollSelected = -1;
// static int scrollTicks = 0;
// static int scrollDelay = 30;
// static int scrollOffset = 0;
// int nameScroller(SDL_Surface *surface, char *path, char *name, char *unique,
//                  int maxWidth, int row, int selected, int reset, int force) {
//   // reset is used when changing directories (otherwise returning from the first
//   // row to the first row above wouldn't reset the scroll)
//   if (reset || selected != scrollSelected) {
//     scrollTicks = 0;
//     scrollOffset = 0;
//     scrollSelected = selected;
//   }

//   scrollTicks += 1;
//   if (scrollTicks < scrollDelay)
//     return 0; // nothing to do yet
//   scrollOffset += 1;

//   SDL_Surface *text;

//   char *displayName = unique ? unique : name;
//   trimSortingMeta(&displayName);

//   if (text->w <= maxWidth) {
//     SDL_FreeSurface(text);
//     return 0;
//   }
//   // prevent overscroll
//   if (scrollOffset > text->w - maxWidth) {
//     scrollOffset = text->w - maxWidth;
//     if (!force) { // nothing to draw unless something outside of this function
//                   // dirtied the screen
//       SDL_FreeSurface(text);
//       return 0;
//     }
//   }

//   SDL_FillRect(surface,
//                &(SDL_Rect){0, 0 + (row * ROW_HEIGHT), SCREEN_WIDTH, ROW_HEIGHT},
//                SDL_MapRGB(surface->format, WHITE));
//   text = TTF_RenderUTF8_Blended(g_font.medium, displayName, DARK_TEXT);

//   int centerY = (ROW_HEIGHT - text->h) / 2;
//   SDL_BlitSurface(text, &(SDL_Rect){0, 0, maxWidth, text->h}, surface,
//                   &(SDL_Rect){PADDING, 0 + (row * ROW_HEIGHT) + centerY});
//   SDL_FreeSurface(text);
//   return 1;
// }

SDL_Surface *loadImage(char *path) {
  static char fullPath[256];
  sprintf(fullPath, "%s/%s", RES_PATH, path);
  SDL_Surface *image = IMG_Load(fullPath);
  if (!image)
    printf("IMG_Load: %s\n", IMG_GetError());
  return image;
}

SDL_Surface *renderText(char *text) {
  SDL_Color color = LIGHT_TEXT;
  return TTF_RenderUTF8_Blended(font.h2, text, color);
}

void hintLabel(SDL_Surface *surface, char *htxt, int x, int y) {
  SDL_Surface *hint_text =
      TTF_RenderUTF8_Blended(font.h3, htxt, LIGHT_TEXT);
  SDL_BlitSurface(hint_text, NULL, surface, &(SDL_Rect){x, y});
  SDL_FreeSurface(hint_text);
}

static void content(TTF_Font* font, char* copy, int lineHeight, SDL_Surface* surface, SDL_Rect* dst_rect) {
	if (dst_rect==NULL) dst_rect = &(SDL_Rect){0,0,surface->w,surface->h};
	#define MAX_TEXT_LINES 16
	char* lines[MAX_TEXT_LINES];
	int count = 0;

	char* tmp;
	lines[count++] = copy;
	while ((tmp=strchr(lines[count-1], '\n'))!=NULL) {
		if (count+1>MAX_TEXT_LINES) break;
		lines[count++] = tmp+1;
	}
	int x = dst_rect->x;
	int y = dst_rect->y;
	
	SDL_Surface* text;
	char line[256];
	for (int i=0; i<count; i++) {
		int len;
		if (i+1<count) {
			len = lines[i+1]-lines[i]-1;
			if (len) strncpy(line, lines[i], len);
			line[len] = '\0'; 
		}
		else {
			len = strlen(lines[i]);
			strcpy(line, lines[i]);
		}
		
		if (len) {
			text = TTF_RenderUTF8_Blended(font, line, LIGHT_TEXT);
			SDL_BlitSurface(text, NULL, surface, &(SDL_Rect){x+((dst_rect->w-text->w)/2),y+(i*lineHeight)});
			SDL_FreeSurface(text);
		}
	}
}

void paragraph(TTF_Font* font, char* msg, int lineHeight, SDL_Surface* surface, SDL_Rect* dst_rect) {
  // NOTE: if you provide a SDL_Rect for position, you must also include the Height and Width
  // for correct position placement, the x and y value would be offsets to the center by default.
  #define TEXT_BOX_MAX_ROWS 16
	if (dst_rect==NULL) dst_rect = &(SDL_Rect){0,0,surface->w,surface->h};
	SDL_Surface* text;
	char* lines[TEXT_BOX_MAX_ROWS];
	int lineCount = 0;

	char* tmp;
	lines[lineCount++] = msg;
	while ((tmp=strchr(lines[lineCount-1], '\n'))!=NULL) {
		if (lineCount+1>=TEXT_BOX_MAX_ROWS) break;
		lines[lineCount++] = tmp+1;
	}
	int rendered_height = lineHeight * lineCount;

	int y = dst_rect->y;
	y += (dst_rect->h - rendered_height) / 2;
	
	char line[256];
	for (int i=0; i<lineCount; i++) {
		int len;
		if (i+1<lineCount) {
			len = lines[i+1]-lines[i]-1;
			if (len) strncpy(line, lines[i], len);
			line[len] = '\0';
		}
		else {
			len = strlen(lines[i]);
			strcpy(line, lines[i]);
		}
		
		
		if (len) {
			text = TTF_RenderUTF8_Blended(font, line, LIGHT_TEXT);
			int x = dst_rect->x;
			x += (dst_rect->w - text->w) / 2;
			SDL_BlitSurface(text, NULL, surface, &(SDL_Rect){x,y});
			SDL_FreeSurface(text);
		}
		y += lineHeight;
	}
}

static void getTextSize(TTF_Font* font, char* str, int lineHeight, int* w, int* h) {
	char* lines[MAX_TEXT_LINES];
	int count = 0;

	char* tmp;
	lines[count++] = str;
	while ((tmp=strchr(lines[count-1], '\n'))!=NULL) {
		if (count+1>MAX_TEXT_LINES) break; // TODO: bail?
		lines[count++] = tmp+1;
	}
	*h = count * lineHeight;
	
	int mw = 0;
	char line[256];
	for (int i=0; i<count; i++) {
		int len;
		if (i+1<count) {
			len = lines[i+1]-lines[i]-1;
			if (len) strncpy(line, lines[i], len);
			line[len] = '\0';
		}
		else {
			len = strlen(lines[i]);
			strcpy(line, lines[i]);
		}
		
		if (len) {
			int lw;
			TTF_SizeUTF8(font, line, &lw, NULL);
			if (lw>mw) mw = lw;
		}
	}
	*w = mw;
}

void inlineText(SDL_Surface *surface, char *str, int x, int y, int dark) {
  SDL_Surface *text;
  SDL_Color font_color = dark ? DARK_TEXT : LIGHT_TEXT;
  text = TTF_RenderUTF8_Blended(font.body, str, font_color);
  SDL_BlitSurface(text, NULL, surface, &(SDL_Rect){x, y});
  SDL_FreeSurface(text);
}

int truncateText(TTF_Font *font, char *displayName, int maxWidth, int padding) {
  int titleWidth;
  TTF_SizeUTF8(font, displayName, &titleWidth, NULL);
  titleWidth += padding;

  while (titleWidth > maxWidth) {
    int len = strlen(displayName);
    strcpy(&displayName[len - 4], "...\0");
    TTF_SizeUTF8(font, displayName, &titleWidth, NULL);
    titleWidth += padding;
  }

  return titleWidth;
}
