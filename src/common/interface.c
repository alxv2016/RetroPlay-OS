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
GFX gfx;
Font font;

void GFX_init(void) {
  TTF_Init();

  font.h1 = TTF_OpenFont(FONT_PATH, H1);
  font.h2 = TTF_OpenFont(FONT_PATH, H2);
  font.h3 = TTF_OpenFont(FONT_PATH, H3);
  font.body = TTF_OpenFont(FONT_PATH, BODY);
  font.caption = TTF_OpenFont(FONT_PATH, CAPTION);
  font.footnote = TTF_OpenFont(FONT_PATH, FOOTNOTE);

  font.h1_500 = TTF_OpenFont(FONT_PATH_MEDIUM, H1);
  font.h2_500 = TTF_OpenFont(FONT_PATH_MEDIUM, H2);
  font.h3_500 = TTF_OpenFont(FONT_PATH_MEDIUM, H3);
  font.body_500 = TTF_OpenFont(FONT_PATH_MEDIUM, BODY);
  font.caption_500 = TTF_OpenFont(FONT_PATH_MEDIUM, CAPTION);
  font.footnote_500 = TTF_OpenFont(FONT_PATH_MEDIUM, FOOTNOTE);

  gfx.button = loadImage("btn.png");
  gfx.button_outline = loadImage("btn-outline.png");
  gfx.dpad = loadImage("menu-btn.png");
  gfx.button_menu = loadImage("d-pad.png");
  gfx.corner_radius = loadImage("radius-black.png");
  gfx.settings_bar_full = loadImage("progress-full.png");
  gfx.settings_bar_empty = loadImage("progress-empty.png");
  gfx.brightness = loadImage("brightness.png");
  gfx.brightness_low = loadImage("brightness-low.png");
  gfx.volume = loadImage("volume.png");
  gfx.mute = loadImage("mute.png");
  gfx.battery_charge = loadImage("battery-charge.png");
  gfx.battery_power = loadImage("battery-power.png");
  gfx.battery = loadImage("battery.png");
  gfx.battery_low_power = loadImage("battery-low-power.png");
  gfx.battery_low = loadImage("battery-low.png");
  gfx.power = loadImage("power.png");
  gfx.sleep = loadImage("sleep.png");
  gfx.recents = loadImage("recents.png");
  gfx.apps = loadImage("apps.png");
  gfx.sleep_timer = loadImage("sleep-timer.png");
  gfx.arcade = loadImage("arcade.png");
  gfx.nes = loadImage("nes.png");
  gfx.gameboy = loadImage("gameboy.png");
  gfx.gba = loadImage("gba.png");
  gfx.gbc = loadImage("gbc.png");
  gfx.sega = loadImage("sega.png");
  gfx.gamegear = loadImage("gamegear.png");
  gfx.playstation = loadImage("playstation.png");
  gfx.snes = loadImage("snes.png");
  gfx.sys_arcade = loadImage("sys-arcade.png");
  gfx.sys_gb = loadImage("sys-gb.png");
  gfx.sys_gba = loadImage("sys-gba.png");
  gfx.sys_gbc = loadImage("sys-gbc.png");
  gfx.sys_gg = loadImage("sys-gg.png");
  gfx.sys_nes = loadImage("sys-nes.png");
  gfx.sys_playstation = loadImage("sys-playstation.png");
  gfx.sys_sega = loadImage("sys-sega.png");
  gfx.sys_snes = loadImage("sys-snes.png");
  gfx.empty_state = loadImage("empty-folder.png");
  gfx.poweroff_state = loadImage("wave.png");
  gfx.sleep_state = loadImage("sleep-mode.png");
}

void GFX_clear(void) {
  SDL_FillRect(gfx.screen, NULL, 0);
  SDL_Flip(gfx.screen);
}

void GFX_ready(void) {
  gfx.screen = SDL_GetVideoSurface(); // :cold_sweat:
}

void GFX_sync(unsigned long frameStart) {
  unsigned long frameDuration = SDL_GetTicks() - frameStart;
  if (frameDuration < FRAME_DURATION) {
    SDL_Delay(FRAME_DURATION - frameDuration);
  }
}

void GFX_quit(void) {
  SDL_FreeSurface(gfx.button);
  SDL_FreeSurface(gfx.button_outline);
  SDL_FreeSurface(gfx.button_menu);
  SDL_FreeSurface(gfx.dpad);
  SDL_FreeSurface(gfx.corner_radius);
  SDL_FreeSurface(gfx.settings_bar_full);
  SDL_FreeSurface(gfx.settings_bar_empty);
  SDL_FreeSurface(gfx.brightness);
  SDL_FreeSurface(gfx.brightness_low);
  SDL_FreeSurface(gfx.volume);
  SDL_FreeSurface(gfx.mute);
  SDL_FreeSurface(gfx.battery_charge);
  SDL_FreeSurface(gfx.battery_power);
  SDL_FreeSurface(gfx.battery);
  SDL_FreeSurface(gfx.battery_low_power);
  SDL_FreeSurface(gfx.battery_low);
  SDL_FreeSurface(gfx.recents);
  SDL_FreeSurface(gfx.apps);
  SDL_FreeSurface(gfx.arcade);
  SDL_FreeSurface(gfx.nes);
  SDL_FreeSurface(gfx.gameboy);
  SDL_FreeSurface(gfx.gamegear);
  SDL_FreeSurface(gfx.gba);
  SDL_FreeSurface(gfx.gbc);
  SDL_FreeSurface(gfx.sega);
  SDL_FreeSurface(gfx.playstation);
  SDL_FreeSurface(gfx.snes);
  SDL_FreeSurface(gfx.sys_arcade);
  SDL_FreeSurface(gfx.sys_gb);
  SDL_FreeSurface(gfx.sys_gba);
  SDL_FreeSurface(gfx.sys_gbc);
  SDL_FreeSurface(gfx.sys_gg);
  SDL_FreeSurface(gfx.sys_nes);
  SDL_FreeSurface(gfx.sys_playstation);
  SDL_FreeSurface(gfx.sys_sega);
  SDL_FreeSurface(gfx.sys_snes);
  SDL_FreeSurface(gfx.empty_state);
  SDL_FreeSurface(gfx.poweroff_state);
  SDL_FreeSurface(gfx.sleep_state);

  TTF_CloseFont(font.h1);
  TTF_CloseFont(font.h2);
  TTF_CloseFont(font.h3);
  TTF_CloseFont(font.body);
  TTF_CloseFont(font.caption);
  TTF_CloseFont(font.footnote);

  TTF_CloseFont(font.h1_500);
  TTF_CloseFont(font.h2_500);
  TTF_CloseFont(font.h3_500);
  TTF_CloseFont(font.body_500);
  TTF_CloseFont(font.caption_500);
  TTF_CloseFont(font.footnote_500);

  if (gfx.screen)
    SDL_FreeSurface(gfx.screen);
  TTF_Quit();
}

/* COMPONENTS */

static void listItem(SDL_Surface *surface, SDL_Surface *icon, int showIcon, char *displayName, int row, int selected_row, int total) {
  #define MIN(min, max) (min) < (max) ? (min) : (max)
  int marginLeft = SPACING_XL;
  int titleMarginLeft = showIcon? marginLeft + CONSOLE_ICON_SIZE + SPACING_MD: marginLeft;
  int availableWidth = SCREEN_WIDTH - marginLeft * 2;
  int titleWidth = truncateText(font.h3, displayName, availableWidth, marginLeft * 2);
  SDL_Surface *title = TTF_RenderUTF8_Blended(font.h3, displayName, (SDL_Color){LIGHT_TEXT});
  int accent = SDL_MapRGB(surface->format, PRIMARY);
  int background = SDL_MapRGB(surface->format, GREY500);

  int rowWidth = showIcon? marginLeft + CONSOLE_ICON_SIZE + SPACING_MD + title->w + marginLeft: marginLeft + title->w + marginLeft;
  int maxTitleWidth = MIN(rowWidth, titleWidth);
  int row_cy = (ROW_HEIGHT / 2) - (title->h / 2);
  int icon_cy = (ROW_HEIGHT / 2) - (CONSOLE_ICON_SIZE / 2);
  // Center list items on screen base on total or max number of rows
  int rows = total < ROW_COUNT? total: ROW_COUNT;
  int screen_center = (SCREEN_HEIGHT / 2) - ((ROW_HEIGHT * rows) / 2);

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
void listMenu(SDL_Surface *surface, char *path, int consoleDir, char *emuTag, char *name, char *unique, int row, int selected_row, int total) {
  SDL_Surface *sysCover;
  char *display_name = unique ? unique : name;
  trimSortingMeta(&display_name);
  sysCover = gfx.sys_arcade;
  int w = sysCover->w;
  int x = SCREEN_WIDTH - (w + SPACING_LG);
  int cy = (SCREEN_HEIGHT / 2) - (sysCover->h / 2);
  // Display console icons on root directory
  if (!strcmp(emuTag, "FBA") && !consoleDir) {
    listItem(surface, gfx.arcade, 1, display_name, row, selected_row, total);
  } else if (!strcmp(emuTag, "FC") && !consoleDir) {
    listItem(surface, gfx.nes, 1, display_name, row, selected_row, total);
  } else if (!strcmp(emuTag, "GB") && !consoleDir) {
    listItem(surface, gfx.gameboy, 1, display_name, row, selected_row, total);
  } else if (!strcmp(emuTag, "GBA") && !consoleDir) {
    listItem(surface, gfx.gba, 1, display_name, row, selected_row, total);
  } else if (!strcmp(emuTag, "GBC") && !consoleDir) {
    listItem(surface,gfx.gbc, 1, display_name, row, selected_row, total);
  } else if (!strcmp(emuTag, "MD") && !consoleDir) {
    listItem(surface, gfx.sega, 1, display_name, row, selected_row, total);
  } else if (!strcmp(emuTag, "GG") && !consoleDir) {
    listItem(surface, gfx.gamegear, 1, display_name, row, selected_row, total);
  } else if (!strcmp(emuTag, "PS") && !consoleDir) {
    listItem(surface, gfx.playstation, 1, display_name, row, selected_row, total);
  } else if (!strcmp(emuTag, "SFC") && !consoleDir) {
    listItem(surface,gfx.snes, 1, display_name, row, selected_row, total);
  } else if (!strcmp(name, "Apps") && !consoleDir) {
    listItem(surface,gfx.apps, 1, display_name, row, selected_row, total);
  } else if (!strcmp(name, "Recently Played")) {
    listItem(surface,gfx.recents, 1, display_name, row, selected_row, total);
  } else {
    // Display system covers images within rom directory
    // if (!strcmp(emuTag, "FBA") && consoleDir) {
    //   SDL_BlitSurface(sysCover, NULL, surface, &(SDL_Rect){x, cy});
    // } else if (!strcmp(emuTag, "FC") && consoleDir) {
    //   sysCover = gfx.sys_nes;
    //   SDL_BlitSurface(sysCover, NULL, surface, &(SDL_Rect){x, cy});
    // } else if (!strcmp(emuTag, "GB") && consoleDir) {
    //   sysCover = gfx.sys_gb;
    //   SDL_BlitSurface(sysCover, NULL, surface, &(SDL_Rect){x, cy});
    // } else if (!strcmp(emuTag, "GBA") && consoleDir) {
    //   sysCover = gfx.sys_gba;
    //   SDL_BlitSurface(sysCover, NULL, surface, &(SDL_Rect){x, cy});
    // } else if (!strcmp(emuTag, "GBC") && consoleDir) {
    //   sysCover = gfx.sys_gbc;
    //   SDL_BlitSurface(sysCover, NULL, surface, &(SDL_Rect){x, cy});
    // } else if (!strcmp(emuTag, "MD") && consoleDir) {
    //   sysCover = gfx.sys_sega;
    //   SDL_BlitSurface(sysCover, NULL, surface, &(SDL_Rect){x, cy});
    // } else if (!strcmp(emuTag, "PS") && consoleDir) {
    //   sysCover = gfx.sys_playstation;
    //   SDL_BlitSurface(sysCover, NULL, surface, &(SDL_Rect){x, cy});
    // } else if (!strcmp(emuTag, "SFC") && consoleDir) {
    //   sysCover = gfx.sys_snes;
    //   SDL_BlitSurface(sysCover, NULL, surface, &(SDL_Rect){x, cy});
    // }
    // Just passing in any dummy icon for non consoles items
    listItem(surface,gfx.nes, 0, display_name, row, selected_row, total);
  }

}

// Battery
void batteryStatus(SDL_Surface *surface, int x, int y) {
  int threshold = 40;
  int charge = getInt(BATTERY_INFO);
  char percentStr[5];
  sprintf(percentStr, "%i%%", charge);
  SDL_Surface *batPower = charge < threshold ? gfx.battery_low_power : gfx.battery_power;
  SDL_Surface *batIcon = gfx.battery;
  SDL_Surface *batLabel = TTF_RenderUTF8_Blended(font.caption_500, percentStr, (SDL_Color){LIGHT_TEXT});
  
  int margin = SPACING_XS;
  int batteryRightAlign = x - batIcon->w;
  int labelRightAlign = x - (batLabel->w + batIcon->w + margin);
  int batLabelCY = (batIcon->h / 2) - (batLabel->h / 2);

  if (isCharging()) {
    // NOTE: Not sure how we can get battery percent during charging.
    batLabel = TTF_RenderUTF8_Blended(font.caption_500, "Charging", (SDL_Color){LIGHT_TEXT});
    labelRightAlign = x - (batLabel->w + batIcon->w + margin);
    SDL_BlitSurface(gfx.battery_charge, NULL, surface, &(SDL_Rect){batteryRightAlign, y});
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

// Primary button
void primaryBTN(SDL_Surface *surface, char *bkey, char *blabel, int rightAlign, int x, int y) {
  SDL_Surface *btn = gfx.button;
  SDL_Surface *btnKey = TTF_RenderUTF8_Blended(font.body_500, bkey, (SDL_Color){DARK_TEXT});
  SDL_Surface *btnLabel = TTF_RenderUTF8_Blended(font.body, blabel, (SDL_Color){LIGHT_TEXT});

  int margin = SPACING_XS;
  int btnCX = (btn->w / 2) - (btnKey->w / 2);
  int btnCY = (btn->h / 2) - (btnKey->h / 2);
  int btnLabelCY = (btn->h / 2) - (btnLabel->h / 2);
  int btnX = BUTTON_SIZE + margin;
  int labelRightAlign = x - btnLabel->w;
  int btnRightAlign = x - (btnLabel->w + btnX);

  SDL_Rect rect;
  rect.x = 0;
  rect.y = 0;
  rect.w = BUTTON_SIZE;
  rect.h = BUTTON_SIZE;

  SDL_BlitSurface(btnLabel, NULL, surface, &(SDL_Rect){rightAlign? labelRightAlign : x, y + btnLabelCY - btn->h});
  SDL_FreeSurface(btnLabel);
  SDL_BlitSurface(btn, &rect, surface, &(SDL_Rect){rightAlign? btnRightAlign : x - btnX, y - btn->h});
  SDL_BlitSurface(btnKey, NULL, surface, &(SDL_Rect){rightAlign? btnRightAlign + btnCX :x + btnCX - btnX, y + btnCY - btn->h});
  SDL_FreeSurface(btnKey);
}
// Secondary button
void secondaryBTN(SDL_Surface *surface, char *bkey, char *blabel, int rightAlign, int x, int y) {
  SDL_Surface *btn = gfx.button_outline;
  SDL_Surface *btnKey = TTF_RenderUTF8_Blended(font.body_500, bkey, (SDL_Color){LIGHT_TEXT});
  SDL_Surface *btnLabel = TTF_RenderUTF8_Blended(font.body, blabel, (SDL_Color){LIGHT_TEXT});

  int margin = SPACING_XS;
  int btnCX = (btn->w / 2) - (btnKey->w / 2);
  int btnCY = (btn->h / 2) - (btnKey->h / 2);
  int btnLabelCY = (btn->h / 2) - (btnLabel->h / 2);
  int btnX = BUTTON_SIZE + margin;
  int labelRightAlign = x - btnLabel->w;
  int btnRightAlign = x - (btnLabel->w + btnX);

  SDL_Rect rect;
  rect.x = 0;
  rect.y = 0;
  rect.w = BUTTON_SIZE;
  rect.h = BUTTON_SIZE;

  SDL_BlitSurface(btnLabel, NULL, surface, &(SDL_Rect){rightAlign? labelRightAlign : x, y + btnLabelCY - btn->h});
  SDL_FreeSurface(btnLabel);
  SDL_BlitSurface(btn, &rect, surface, &(SDL_Rect){rightAlign? btnRightAlign : x - btnX, y - btn->h});
  SDL_BlitSurface(btnKey, NULL, surface, &(SDL_Rect){rightAlign? btnRightAlign + btnCX :x + btnCX - btnX, y + btnCY - btn->h});
  SDL_FreeSurface(btnKey);
}
// Tertiary button
void tertiaryBTN(SDL_Surface *surface, char *blabel, int altUI, int rightAlign, int x, int y) {
  SDL_Surface *btn = altUI? gfx.button_menu : gfx.dpad;
  SDL_Surface *btnLabel = TTF_RenderUTF8_Blended(font.body, blabel, (SDL_Color){LIGHT_TEXT});

  int margin = SPACING_XS;
  int btnCX = (btn->w / 2);
  // Bump 1px up to visually center letter in btn
  int btnCY = (btn->h / 2);
  int btnLabelCY = (btn->h / 2) - (btnLabel->h / 2);
  int btnX = BUTTON_SIZE + margin;
  int labelRightAlign = x - btnLabel->w;
  int btnRightAlign = x - (btnLabel->w + btnX);

  SDL_Rect rect;
  rect.x = 0;
  rect.y = 0;
  rect.w = BUTTON_SIZE;
  rect.h = BUTTON_SIZE;

  SDL_BlitSurface(btnLabel, NULL, surface, &(SDL_Rect){rightAlign? labelRightAlign : x, y + btnLabelCY - btn->h});
  SDL_FreeSurface(btnLabel);
  SDL_BlitSurface(btn, &rect, surface, &(SDL_Rect){rightAlign? btnRightAlign : x - btnX, y - btn->h});
}

static int calcProgress(int width, int value, int minValue, int maxValue) {
  int progress = width * ((float)(value - minValue) / (maxValue - minValue));
  return progress;
};
// Volumn control component
void volumeControl(SDL_Surface *surface, int x, int y, int bottomAlign, int value, int minValue, int maxValue) {
  SDL_Surface *icon = value == 0? gfx.mute: gfx.volume;
  int marginLeft = ICON_SIZE + SPACING_MD;
  int w = SCREEN_WIDTH / 2 - marginLeft;
  int h = 4;
  int pw = calcProgress(w, value, minValue, maxValue);
  int cy = (icon->h / 2) - (h / 2);
  int progress = SDL_MapRGB(surface->format, WHITE);
  int background = SDL_MapRGB(surface->format, GREY400);

  SDL_BlitSurface(icon, NULL, surface, &(SDL_Rect){x, bottomAlign? y - icon->h: y});
  SDL_FillRect(surface, &(SDL_Rect){x + marginLeft, bottomAlign? y + cy - icon->h: y + cy, w, h}, background);
  SDL_FillRect(surface, &(SDL_Rect){x + marginLeft, bottomAlign? y + cy - icon->h: y+ cy, pw, h}, progress);
}
// Brightness control component
void brightnessControl(SDL_Surface *surface, int x, int y, int value, int minValue, int maxValue) {
  SDL_Surface *icon = value < 5? gfx.brightness_low: gfx.brightness;
  int marginLeft = ICON_SIZE + SPACING_MD;
  int w = SCREEN_WIDTH / 2 - marginLeft;
  int h = 4;
  int pw = calcProgress(w, value, minValue, maxValue);
  int cy = (icon->h / 2) - (h / 2);
  int progress = SDL_MapRGB(surface->format, WHITE);
  int background = SDL_MapRGB(surface->format, GREY400);

  SDL_BlitSurface(icon, NULL, surface, &(SDL_Rect){x, y});
  SDL_FillRect(surface, &(SDL_Rect){x + marginLeft, y + cy, w, h}, background);
  SDL_FillRect(surface, &(SDL_Rect){x + marginLeft, y + cy, pw, h}, progress);
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
  return TTF_RenderUTF8_Blended(font.h2, text, (SDL_Color){LIGHT_TEXT});
}

void heading(int fontSize, int bold, char* copy, SDL_Color color, SDL_Surface *surface, SDL_Rect *offset) {
  // NOTE: if you provide a SDL_Rect for position, you must also include the Height and Width
  // for correct position placement, the x and y value would be offsets to the center.
  TTF_Font *headingStyle;
  int maxLines = 16;
  int lineHeight = fontSize * HEADING_LINEHEIGHT;
	if (offset==NULL) offset = &(SDL_Rect){0,0,surface->w,surface->h};

  switch(fontSize) {
    case H1:
    headingStyle = bold? font.h1_500 : font.h1;
    break;
    case H2:
    headingStyle = bold? font.h2_500 : font.h2;
    break;
    case H3:
    headingStyle = bold? font.h3_500 : font.h3;
    break;
  }

	SDL_Surface* text;
	char* lines[maxLines];
	int lineCount = 0;

	char* tmp;
	lines[lineCount++] = copy;
	while ((tmp=strchr(lines[lineCount-1], '\n'))!=NULL) {
		if (lineCount+1>=maxLines) break;
		lines[lineCount++] = tmp+1;
	}
	int rendered_height = lineHeight * lineCount;

	int y = offset->y;
	y += (offset->h - rendered_height) / 2;
	
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
			text = TTF_RenderUTF8_Blended(headingStyle, line, color);
			int x = offset->x;
			x += (offset->w - text->w) / 2;
			SDL_BlitSurface(text, NULL, surface, &(SDL_Rect){x,y});
			SDL_FreeSurface(text);
		}
		y += lineHeight;
	}
}

void paragraph(int fontSize, int bold, char* copy, SDL_Color color, SDL_Surface *surface, SDL_Rect *offset) {
  // NOTE: if you provide a SDL_Rect for position, you must also include the Height and Width
  // for correct position placement, the x and y value would be offsets to the center.
  TTF_Font *paragraphStyle;
  int maxLines = 16;
  int lineHeight = fontSize * BODY_LINEHEIGHT;
	if (offset==NULL) offset = &(SDL_Rect){0,0,surface->w,surface->h};

  switch(fontSize) {
    case BODY:
    paragraphStyle = bold? font.body_500: font.body;
    break;
    case CAPTION:
    paragraphStyle = bold? font.caption_500: font.caption;
    break;
    case FOOTNOTE:
    paragraphStyle = bold? font.footnote_500: font.footnote;
    break;
  }

	SDL_Surface* text;
	char* lines[maxLines];
	int lineCount = 0;

	char* tmp;
	lines[lineCount++] = copy;
	while ((tmp=strchr(lines[lineCount-1], '\n'))!=NULL) {
		if (lineCount+1>=maxLines) break;
		lines[lineCount++] = tmp+1;
	}
	int rendered_height = lineHeight * lineCount;

	int y = offset->y;
	y += (offset->h - rendered_height) / 2;
	
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
			text = TTF_RenderUTF8_Blended(paragraphStyle, line, color);
			int x = offset->x;
			x += (offset->w - text->w) / 2;
			SDL_BlitSurface(text, NULL, surface, &(SDL_Rect){x,y});
			SDL_FreeSurface(text);
		}
		y += lineHeight;
	}
}

void emptyState(SDL_Surface *surface, int headingSize, int bodySize, char *headingCopy, char *bodyCopy) {
  SDL_Surface *emptyStateIcon = gfx.empty_state;
  TTF_Font *headingStyle;
  int margin = SPACING_XXS;
  int headingHeight;
  switch(headingSize) {
    case H1:
    headingStyle = font.h1;
    break;
    case H2:
    headingStyle = font.h2;
    break;
    case H3:
    headingStyle = font.h3;
    break;
  }

  TTF_SizeUTF8(headingStyle, headingCopy, NULL, &headingHeight);
  int cx = (SCREEN_WIDTH / 2) - (emptyStateIcon->w / 2);
  int cy = (SCREEN_HEIGHT / 2) - (emptyStateIcon->h / 2);

  SDL_BlitSurface(emptyStateIcon, NULL, surface, &(SDL_Rect){cx, cy - (emptyStateIcon->h / 2)});
  heading(headingSize, 1, headingCopy, (SDL_Color){LIGHT_TEXT}, surface, &(SDL_Rect){0, SPACING_XL, surface->w, surface->h});
  paragraph(bodySize, 0, bodyCopy, (SDL_Color){NEUTRAL_TEXT}, surface, &(SDL_Rect){0, headingHeight + SPACING_XL + margin, surface->w, surface->h});
}

void powerOffState(SDL_Surface *surface, char *headingCopy, char *bodyCopy) {
  SDL_Surface *powerOffStateIcon = gfx.poweroff_state;
  int margin = SPACING_XXS;
  int headingHeight;

  TTF_SizeUTF8(font.h1, headingCopy, NULL, &headingHeight);
  int cx = (SCREEN_WIDTH / 2) - (powerOffStateIcon->w / 2);
  int cy = (SCREEN_HEIGHT / 2) - (powerOffStateIcon->h / 2);

  SDL_BlitSurface(powerOffStateIcon, NULL, surface, &(SDL_Rect){cx, cy - (powerOffStateIcon->h / 2)});
  heading(H1, 1, headingCopy, (SDL_Color){LIGHT_TEXT}, surface, &(SDL_Rect){0, SPACING_XL, surface->w, surface->h});
  paragraph(BODY, 0, bodyCopy, (SDL_Color){NEUTRAL_TEXT}, surface, &(SDL_Rect){0, headingHeight + SPACING_XL + margin, surface->w, surface->h});
}

void sleepState(SDL_Surface *surface, char *headingCopy, char *bodyCopy) {
  SDL_Surface *icon = gfx.sleep_state;
  int margin = SPACING_XXS;
  int headingHeight;

  TTF_SizeUTF8(font.h1, headingCopy, NULL, &headingHeight);
  int cx = (SCREEN_WIDTH / 2) - (icon->w / 2);
  int cy = (SCREEN_HEIGHT / 2) - (icon->h / 2);

  SDL_BlitSurface(icon, NULL, surface, &(SDL_Rect){cx, cy - (icon->h / 2)});
  heading(H1, 1, headingCopy, (SDL_Color){LIGHT_TEXT}, surface, &(SDL_Rect){0, SPACING_XL, surface->w, surface->h});
  paragraph(BODY, 0, bodyCopy, (SDL_Color){NEUTRAL_TEXT}, surface, &(SDL_Rect){0, headingHeight + SPACING_XL + margin, surface->w, surface->h});
}

static void getTextSize(TTF_Font* font, char* str, int lineHeight, int* w, int* h) {
	char* lines[16];
	int count = 0;

	char* tmp;
	lines[count++] = str;
	while ((tmp=strchr(lines[count-1], '\n'))!=NULL) {
		if (count+1>16) break; // TODO: bail?
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

/* DEPRECATED COMPONENTS */

// Pill button deprecated
// void pillButton(SDL_Surface *surface, char *bkey, char *blabel, int x, int y) {
//   SDL_Surface *btn = gfx.button;
//   SDL_Surface *btnKey =
//       TTF_RenderUTF8_Blended(font.body, bkey, (SDL_Color){DARK_TEXT});
//   SDL_Surface *btnLabel =
//       TTF_RenderUTF8_Blended(font.body, blabel, (SDL_Color){LIGHT_TEXT});

//   // Pill's left radius
//   SDL_Rect rectL;
//   rectL.x = 0;
//   rectL.y = 0;
//   rectL.w = BUTTON_SIZE / 2;
//   rectL.h = BUTTON_SIZE;

//   SDL_Rect rectFill;
//   rectFill.x = x + BUTTON_SIZE / 2;
//   rectFill.y = y;
//   rectFill.w = btnKey->w;
//   rectFill.h = BUTTON_SIZE;

//   SDL_Rect rectR;
//   rectR.x = BUTTON_SIZE / 2;
//   rectR.y = 0;
//   rectR.w = BUTTON_SIZE / 2;
//   rectR.h = BUTTON_SIZE;

//   int margin = 6;
//   int btnCX = ((btn->w / 2) + (btnKey->w / 2)) - (btnKey->w / 2);
//   // Bump 2px up to visually center letter in btn
//   int btnCY = (btn->h / 2) - (btnKey->h / 2) - 2;
//   int btnLabelCY = (btn->h / 2) - (btnLabel->h / 2);
//   int btnX = btn->w / 2 + btnKey->w + btn->w / 2 + margin;

//   SDL_BlitSurface(btn, &rectL, surface, &(SDL_Rect){x, y});
//   // Pill's fill container
//   SDL_FillRect(surface, &rectFill, SDL_MapRGB(btn->format, WHITE));
//   SDL_BlitSurface(btn, &rectR, surface,
//                   &(SDL_Rect){x + btn->w / 2 + btnKey->w, y});
//   SDL_BlitSurface(btnKey, NULL, surface, &(SDL_Rect){x + btnCX, y + btnCY});
//   SDL_FreeSurface(btnKey);

//   SDL_BlitSurface(btnLabel, NULL, surface,
//                   &(SDL_Rect){x + btnX, y + btnLabelCY});
//   SDL_FreeSurface(btnLabel);
// }

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