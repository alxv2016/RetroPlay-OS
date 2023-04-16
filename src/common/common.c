#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

#include <msettings.h>

#include "defines.h"
#include "utils.h"
#include "powerops.h"
#include "common.h"

///////////////////////////////////////

///////////////////////////////////////

int GFX_truncateText(TTF_Font *font, const char *in_name, char *out_name, int max_width, int padding)
{
	int text_width;
	strcpy(out_name, in_name);
	TTF_SizeUTF8(font, out_name, &text_width, NULL);
	text_width += padding;

	while (text_width > max_width)
	{
		int len = strlen(out_name);
		strcpy(&out_name[len - 4], "...\0");
		TTF_SizeUTF8(font, out_name, &text_width, NULL);
		text_width += padding;
	}

	return text_width;
}
///////////////////////////////////////////////////////////////
// Video screen surface
SDL_Surface *screen;
// UI Elements
static struct GFX_Context
{
	SDL_Surface *button;
	SDL_Surface *slot_overlay;
	SDL_Surface *bg_white;
	SDL_Surface *corner_radius;
	SDL_Surface *settings_bar_full;
	SDL_Surface *settings_bar_empty;
	SDL_Surface *brightness;
	SDL_Surface *volume;
	SDL_Surface *mute;
	SDL_Surface *battery_charge;
	SDL_Surface *battery_power;
	SDL_Surface *battery;
	SDL_Surface *battery_low_power;
	SDL_Surface *battery_low;
	// Ingame menu assets
	SDL_Surface *overlay;
	SDL_Surface *slot_pagination;
	SDL_Surface *slot_active;
	SDL_Surface *Slot_preview;
	SDL_Surface *no_preview;
	SDL_Surface *empty_slot;
} gfx;
// Fonts
static struct GFX_Fonts
{
	TTF_Font *large;
	TTF_Font *medium;
	TTF_Font *small;
	TTF_Font *tiny;
} font;
///////////////////////////////////////////////////////////////
// Graphics Initialization
void GFX_init(void)
{
	TTF_Init();
	font.large = TTF_OpenFont(FONT_PATH, FONT_LARGE);
	font.medium = TTF_OpenFont(FONT_PATH, FONT_MEDIUM);
	font.small = TTF_OpenFont(FONT_PATH, FONT_SMALL);
	font.tiny = TTF_OpenFont(FONT_PATH, FONT_TINY);

	gfx.button = GFX_loadImage("btn.png");
	gfx.bg_white = GFX_loadImage("bg-white.png");
	gfx.corner_radius = GFX_loadImage("radius-black.png");
	gfx.settings_bar_full = GFX_loadImage("settings-bar-full.png");
	gfx.settings_bar_empty = GFX_loadImage("settings-bar-empty.png");
	gfx.brightness = GFX_loadImage("brightness.png");
	gfx.volume = GFX_loadImage("volume.png");
	gfx.mute = GFX_loadImage("mute.png");
	gfx.battery_charge = GFX_loadImage("battery-charge.png");
	gfx.battery_power = GFX_loadImage("battery-power.png");
	gfx.battery = GFX_loadImage("battery.png");
	gfx.battery_low_power = GFX_loadImage("battery-low-power.png");
	gfx.battery_low = GFX_loadImage("battery-low.png");
}

void GFX_menuInit(void)
{
	// Ingame Menu assets
	gfx.slot_overlay = GFX_loadImage("slot-overlay.png");
	gfx.slot_pagination = GFX_loadImage("slot-dots.png");
	gfx.slot_active = GFX_loadImage("slot-dot-selected.png");
	gfx.no_preview = GFX_getText("No Preview");
	gfx.empty_slot = GFX_getText("Empty Slot");
}

void GFX_clear(void)
{
	SDL_FillRect(screen, NULL, 0);
	SDL_Flip(screen);
}

void GFX_ready(void)
{
	screen = SDL_GetVideoSurface(); // :cold_sweat:
}

void GFX_quit(void)
{
	SDL_FreeSurface(gfx.button);
	SDL_FreeSurface(gfx.bg_white);
	SDL_FreeSurface(gfx.corner_radius);
	SDL_FreeSurface(gfx.settings_bar_full);
	SDL_FreeSurface(gfx.settings_bar_empty);
	SDL_FreeSurface(gfx.brightness);
	SDL_FreeSurface(gfx.volume);
	SDL_FreeSurface(gfx.mute);
	SDL_FreeSurface(gfx.battery_charge);
	SDL_FreeSurface(gfx.battery_power);
	SDL_FreeSurface(gfx.battery);
	SDL_FreeSurface(gfx.battery_low_power);
	SDL_FreeSurface(gfx.battery_low);

	TTF_CloseFont(font.large);
	TTF_CloseFont(font.medium);
	TTF_CloseFont(font.small);
	TTF_CloseFont(font.tiny);

	if (screen)
		SDL_FreeSurface(screen);
	TTF_Quit();
}
// Ingame Menu
void GFX_menuQuit(void)
{
	SDL_FreeSurface(gfx.overlay);
	SDL_FreeSurface(gfx.slot_overlay);
	SDL_FreeSurface(gfx.slot_pagination);
	SDL_FreeSurface(gfx.slot_active);
	SDL_FreeSurface(gfx.no_preview);
	SDL_FreeSurface(gfx.empty_slot);
}

///////////////////////////////////////////////////////////////
// Graphics utilities functions
SDL_Surface *GFX_loadImage(char *path)
{
	static char full_path[256];
	sprintf(full_path, "%s/%s", RES_PATH, path);
	SDL_Surface *image = IMG_Load(full_path);
	if (!image)
		printf("IMG_Load: %s\n", IMG_GetError());
	return image;
}

SDL_Surface *GFX_getText(char *text)
{
	SDL_Color color = COLOR_LIGHT_TEXT;
	return TTF_RenderUTF8_Blended(font.medium, text, color);
}

int GFX_getButtonWidth(char *blabel, char *bkey)
{
	int margin = 8;
	int button_width = 0;
	int compute_width;

	if (strlen(bkey) == 1)
	{
		button_width += BUTTON_SIZE;
	}
	else
	{
		button_width += BUTTON_SIZE;
		TTF_SizeUTF8(font.medium, bkey, &compute_width, NULL);
		button_width += compute_width;
	}
	button_width += margin;

	TTF_SizeUTF8(font.small, blabel, &compute_width, NULL);
	button_width += compute_width + margin;
	return button_width;
}

// Long name scroll *Deprecate? May need to refactor if use
static int scroll_selected = -1;
static int scroll_ticks = 0;
static int scroll_delay = 30;
static int scroll_ox = 0;
int GFX_scrollLongNames(SDL_Surface *surface, char *name, char *path, char *unique, int row, int selected, int reset, int force)
{
	int max_width = SCREEN_WIDTH - PADDING;
	// reset is used when changing directories (otherwise returning from the first row to the first row above wouldn't reset the scroll)
	if (reset || selected != scroll_selected)
	{
		scroll_ticks = 0;
		scroll_ox = 0;
		scroll_selected = selected;
	}

	scroll_ticks += 1;
	if (scroll_ticks < scroll_delay)
		return 0; // nothing to do yet
	scroll_ox += 1;

	SDL_Surface *text;

	char *display_name = unique ? unique : name;
	trimSortingMeta(&display_name);

	if (text->w <= max_width)
	{
		SDL_FreeSurface(text);
		return 0;
	}
	// prevent overscroll
	if (scroll_ox > text->w - max_width)
	{
		scroll_ox = text->w - max_width;
		if (!force)
		{ // nothing to draw unless something outside of this function dirtied the screen
			SDL_FreeSurface(text);
			return 0;
		}
	}

	SDL_FillRect(surface, &(SDL_Rect){0, 0 + (row * ROW_HEIGHT), SCREEN_WIDTH, ROW_HEIGHT}, SDL_MapRGB(surface->format, TRIAD_WHITE));
	text = TTF_RenderUTF8_Blended(font.medium, display_name, COLOR_DARK_TEXT);

	int center_y = (ROW_HEIGHT - text->h) / 2;
	SDL_BlitSurface(text, &(SDL_Rect){0, 0, max_width, text->h}, surface, &(SDL_Rect){PADDING, 0 + (row * ROW_HEIGHT) + center_y});
	SDL_FreeSurface(text);
	return 1;
}
///////////////////////////////////////////////////////////////
// UI Components

// Battery
void GFX_blitBattery(SDL_Surface *surface, int x, int y)
{
	int charge = getInt("/tmp/battery");
	SDL_Surface *bat_power = charge <= 50 ? gfx.battery_low_power : gfx.battery_power;
	SDL_Surface *bat_icon = gfx.battery;
	SDL_Surface *bat_label;

	char percent_str[5];
	sprintf(percent_str, "%i%%", charge);

	bat_label = TTF_RenderUTF8_Blended(font.small, percent_str, COLOR_LIGHT_TEXT);
	int margin_right = bat_label->w + 8;
	int bat_label_cy = (bat_icon->h / 2) - (bat_label->h / 2);

	if (isCharging())
	{
		// NOTE: Not sure how we can get battery percent during charging.
		bat_label = TTF_RenderUTF8_Blended(font.small, "Charging", COLOR_LIGHT_TEXT);
		int margin_right = bat_label->w + 8;
		SDL_BlitSurface(gfx.battery_charge, NULL, surface, &(SDL_Rect){x, y});
		SDL_BlitSurface(bat_label, NULL, surface, &(SDL_Rect){x - margin_right, y + bat_label_cy});
		SDL_FreeSurface(bat_label);
	}
	else
	{
		int pwr_width = 24;
		int pwr_height = 12;
		int pwr_amount = pwr_width * charge / 100;
		// 2 is 2px right-offset from the center of the battery image
		int cx = (bat_icon->w / 2) - (bat_power->w / 2) - 1;
		int cy = (bat_icon->h / 2) - (bat_power->h / 2);
		int margin_left = bat_label->w + 8;
		int bat_label_cy = (bat_icon->h / 2) - (bat_label->h / 2);

		SDL_Rect rect;
		rect.x = 0;
		rect.y = 0;
		rect.w = pwr_amount;
		rect.h = pwr_height;

		SDL_BlitSurface(bat_icon, NULL, surface, &(SDL_Rect){x, y});
		SDL_BlitSurface(bat_power, &rect, surface, &(SDL_Rect){x + cx, y + cy});
		SDL_BlitSurface(bat_label, NULL, surface, &(SDL_Rect){x - margin_right, y + bat_label_cy});
		SDL_FreeSurface(bat_label);
	}
}

// Hint copy *Deprecating
void GFX_blitHint(SDL_Surface *surface, char *htxt, int x, int y)
{
	SDL_Surface *hint_text = TTF_RenderUTF8_Blended(font.small, htxt, COLOR_LIGHT_TEXT);
	SDL_BlitSurface(hint_text, NULL, surface, &(SDL_Rect){x, y});
	SDL_FreeSurface(hint_text);
}

// Pill component
void GFX_blitPill(SDL_Surface *surface, char *bkey, char *blabel, int x, int y)
{
	SDL_Surface *btn = gfx.button;
	SDL_Surface *btn_key = TTF_RenderUTF8_Blended(font.medium, bkey, COLOR_DARK_TEXT);
	SDL_Surface *btn_label = TTF_RenderUTF8_Blended(font.small, blabel, COLOR_LIGHT_TEXT);

	// Pill's left radius
	SDL_Rect rect_l;
	rect_l.x = 0;
	rect_l.y = 0;
	rect_l.w = BUTTON_SIZE / 2;
	rect_l.h = BUTTON_SIZE;

	SDL_Rect rect_fill;
	rect_fill.x = x + BUTTON_SIZE / 2;
	rect_fill.y = y;
	rect_fill.w = btn_key->w;
	rect_fill.h = BUTTON_SIZE;

	SDL_Rect rect_r;
	rect_r.x = BUTTON_SIZE / 2;
	rect_r.y = 0;
	rect_r.w = BUTTON_SIZE / 2;
	rect_r.h = BUTTON_SIZE;

	int margin = 8;
	int btn_cx = ((btn->w / 2) + (btn_key->w / 2)) - (btn_key->w / 2);
	// Bump 2px up to visually center letter in btn
	int btn_cy = (btn->h / 2) - (btn_key->h / 2) - 2;
	int btn_label_cy = (btn->h / 2) - (btn_label->h / 2);
	int btn_x = btn->w / 2 + btn_key->w + btn->w / 2 + margin;

	SDL_BlitSurface(btn, &rect_l, surface, &(SDL_Rect){x, y});
	// Pill's fill container
	SDL_FillRect(surface, &rect_fill, SDL_MapRGB(btn->format, TRIAD_WHITE));
	SDL_BlitSurface(btn, &rect_r, surface, &(SDL_Rect){x + btn->w / 2 + btn_key->w, y});
	SDL_BlitSurface(btn_key, NULL, surface, &(SDL_Rect){x + btn_cx, y + btn_cy});
	SDL_FreeSurface(btn_key);

	SDL_BlitSurface(btn_label, NULL, surface, &(SDL_Rect){x + btn_x, y + btn_label_cy});
	SDL_FreeSurface(btn_label);
}

// Button component
void GFX_blitButton(SDL_Surface *surface, char *bkey, char *blabel, int x, int y)
{
	SDL_Surface *btn = gfx.button;
	SDL_Surface *btn_key = TTF_RenderUTF8_Blended(font.medium, bkey, COLOR_DARK_TEXT);
	SDL_Surface *btn_label = TTF_RenderUTF8_Blended(font.small, blabel, COLOR_LIGHT_TEXT);

	int margin = 8;
	int btn_cx = (btn->w / 2) - (btn_key->w / 2);
	// Bump 2px up to visually center letter in btn
	int btn_cy = (btn->h / 2) - (btn_key->h / 2) - 1;
	int btn_label_cy = (btn->h / 2) - (btn_label->h / 2);
	int btn_x = BUTTON_SIZE + margin;

	SDL_Rect rect;
	rect.x = 0;
	rect.y = 0;
	rect.w = BUTTON_SIZE;
	rect.h = BUTTON_SIZE;

	SDL_BlitSurface(btn_label, NULL, surface, &(SDL_Rect){x, y + btn_label_cy});
	SDL_FreeSurface(btn_label);

	SDL_BlitSurface(btn, &rect, surface, &(SDL_Rect){x - btn_x, y});
	SDL_BlitSurface(btn_key, NULL, surface, &(SDL_Rect){x + btn_cx - btn_x, y + btn_cy});
	SDL_FreeSurface(btn_key);
}

// Volumn settings component
void GFX_blitSettings(SDL_Surface *surface, int x, int y, int icon, int value, int min_value, int max_value)
{
	SDL_Surface *s_icon = icon == 0 ? gfx.brightness : (icon == 1 ? gfx.volume : gfx.mute);
	SDL_Surface *s_progress_empty = gfx.settings_bar_empty;
	SDL_Surface *s_progress_bar = gfx.settings_bar_full;

	int cy = (s_icon->h / 2) - (s_progress_empty->h / 2);
	int w = s_progress_bar->w * ((float)(value - min_value) / (max_value - min_value));
	int h = s_progress_bar->h;
	int margin_left = ICON_SIZE + 8;

	GFX_blitIngameWindow(surface, x, y, SCREEN_WIDTH / 2, ICON_SIZE * 2);

	SDL_BlitSurface(s_icon, NULL, surface, &(SDL_Rect){x, y});
	SDL_BlitSurface(s_progress_empty, NULL, surface, &(SDL_Rect){x + margin_left, y + cy});
	SDL_BlitSurface(s_progress_bar, &(SDL_Rect){0, 0, w, h}, surface, &(SDL_Rect){x + margin_left, y + cy, w, h});
}
// Menu list component
void GFX_blitMainMenu(SDL_Surface *surface, char *name, char *path, char *unique, int row, int selected_row)
{
#define MIN(a, b) (a) < (b) ? (a) : (b)
	char *display_name = unique ? unique : name;
	trimSortingMeta(&display_name);
	int margin_left = 32;

	SDL_Surface *text;
	text = TTF_RenderUTF8_Blended(font.small, display_name, COLOR_LIGHT_TEXT);
	int row_width = text->w + margin_left * 2;
	int text_width = GFX_truncateText(font.small, display_name, display_name, SCREEN_WIDTH / 2, margin_left) + margin_left;
	int max_width = MIN(row_width, text_width);
	int row_cy = (ROW_HEIGHT / 2) - (text->h / 2);
	int screen_center = (SCREEN_HEIGHT / 2) - ((ROW_HEIGHT * ROW_COUNT) / 2);

	if (row == selected_row)
	{
		// Selected rows
		text = TTF_RenderUTF8_Blended(font.small, display_name, COLOR_DARK_TEXT);
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

// Ingame menu component
void GFX_blitIngameWindow(SDL_Surface *surface, int x, int y, int width, int height)
{
	int radius_w = (RADIUS / 2);
	int radius_h = (RADIUS / 2);
	// Window corner radius
	SDL_BlitSurface(gfx.corner_radius, &(SDL_Rect){0, 0, radius_w, radius_h}, surface, &(SDL_Rect){x, y});
	SDL_BlitSurface(gfx.corner_radius, &(SDL_Rect){radius_w, 0, radius_w, radius_h}, surface, &(SDL_Rect){x + width - radius_w, y});
	SDL_BlitSurface(gfx.corner_radius, &(SDL_Rect){0, radius_h, radius_w, radius_h}, surface, &(SDL_Rect){x, y + height - radius_h});
	SDL_BlitSurface(gfx.corner_radius, &(SDL_Rect){radius_w, radius_h, radius_w, radius_h}, surface, &(SDL_Rect){x + width - radius_w, y + height - radius_h});

	// Window fill
	SDL_FillRect(surface, &(SDL_Rect){x + radius_w, y, width - RADIUS, radius_h}, SDL_MapRGB(surface->format, TRIAD_BLACK));
	SDL_FillRect(surface, &(SDL_Rect){x, y + radius_h, width, height - RADIUS}, SDL_MapRGB(surface->format, TRIAD_BLACK));
	SDL_FillRect(surface, &(SDL_Rect){x + radius_w, y + height - radius_h, width - RADIUS, radius_h}, SDL_MapRGB(surface->format, TRIAD_BLACK));
}

// Long paragaphs text contents, use "\n" to drop to new line
void GFX_blitParagraph(SDL_Surface *surface, char *str, int x, int y, int width, int height)
{
	SDL_Surface *text;
	char *rows[MAX_ROW];
	int row_count = 0;

	char *tmp;
	rows[row_count++] = str;
	while ((tmp = strchr(rows[row_count - 1], '\n')) != NULL)
	{
		if (row_count + 1 >= MAX_ROW)
			return;
		rows[row_count++] = tmp + 1;
	}

	int rendered_height = FONT_LINEHEIGHT * row_count;
	y += (height - rendered_height) / 2;

	char line[MAX_PATH];
	for (int i = 0; i < row_count; i++)
	{
		int len;
		if (i + 1 < row_count)
		{
			len = rows[i + 1] - rows[i] - 1;
			if (len)
				strncpy(line, rows[i], len);
			line[len] = '\0';
		}
		else
		{
			len = strlen(rows[i]);
			strcpy(line, rows[i]);
		}

		if (len)
		{
			text = TTF_RenderUTF8_Blended(font.medium, line, COLOR_WHITE);
			x += (width - text->w) / 2;
			SDL_BlitSurface(text, NULL, surface, &(SDL_Rect){x, y});
			SDL_FreeSurface(text);
		}
		y += FONT_LINEHEIGHT;
	}
}

// Text component
void GFX_blitText(SDL_Surface *surface, char *str, int x, int y, int color)
{
	SDL_Surface *text;
	SDL_Color font_color = color ? COLOR_DARK_TEXT : COLOR_LIGHT_TEXT;
	text = TTF_RenderUTF8_Blended(font.medium, str, font_color);
	SDL_BlitSurface(text, NULL, surface, &(SDL_Rect){x, y});
	SDL_FreeSurface(text);
}

///////////////////////////////////////////////////////////////
// Power and battery logic