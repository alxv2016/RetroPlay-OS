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
#include "common.h"
#include "defines.h"
#include "utils.h"

///////////////////////////////////////
// GamePad events
struct GamePadContext
{
	int is_pressed;
	int just_pressed;
	int just_repeated;
	int just_released;
};
static struct GamePadContext gamePad[BTN_COUNT];

void Input_reset(void)
{
	for (int i = 0; i < BTN_COUNT; i++)
	{
		gamePad[i].is_pressed = CODE_NONE;
		gamePad[i].just_pressed = CODE_NONE;
		gamePad[i].just_repeated = CODE_NONE;
		gamePad[i].just_released = CODE_NONE;
	}
}

void Input_poll(void)
{
	// reset transient values
	for (int i = 0; i < BTN_COUNT; i++)
	{
		gamePad[i].just_pressed = CODE_NONE;
		gamePad[i].just_repeated = CODE_NONE;
		gamePad[i].just_released = CODE_NONE;
	}

	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		GamePadIndex i;
		if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)
		{
			SDLKey key = event.key.keysym.sym;
			// NOTE: can't use switch because all missing buttons have the same value
			if (key == CODE_A)
				i = BTN_A;
			else if (key == CODE_B)
				i = BTN_B;
			else if (key == CODE_X)
				i = BTN_X;
			else if (key == CODE_Y)
				i = BTN_Y;
			else if (key == CODE_START)
				i = BTN_START;
			else if (key == CODE_SELECT)
				i = BTN_SELECT;
			else if (key == CODE_UP)
				i = BTN_UP;
			else if (key == CODE_DOWN)
				i = BTN_DOWN;
			else if (key == CODE_LEFT)
				i = BTN_LEFT;
			else if (key == CODE_RIGHT)
				i = BTN_RIGHT;
			else if (key == CODE_L1)
				i = BTN_L1;
			else if (key == CODE_R1)
				i = BTN_R1;
			else if (key == CODE_MENU)
				i = BTN_MENU;
			else if (key == CODE_L2)
				i = BTN_L2;
			else if (key == CODE_R2)
				i = BTN_R2;
			else if (key == CODE_PLUS)
				i = BTN_PLUS;
			else if (key == CODE_MINUS)
				i = BTN_MINUS;
			else if (key == CODE_POWER)
				i = BTN_POWER;
			else
				continue;

			if (event.type == SDL_KEYDOWN)
			{
				gamePad[i].just_repeated = 1;
				if (!gamePad[i].is_pressed)
				{
					gamePad[i].just_pressed = 1;
					gamePad[i].is_pressed = 1;
				}
			}
			else
			{
				gamePad[i].is_pressed = 0;
				gamePad[i].just_released = 1;
			}
		}
	}
}

int Input_anyPressed(void)
{
	for (int i = 0; i < BTN_COUNT; i++)
	{
		if (gamePad[i].is_pressed)
			return 1;
	}
	return 0;
}
int Input_justPressed(GamePadIndex btn) { return gamePad[btn].just_pressed; }
int Input_justRepeated(GamePadIndex btn) { return gamePad[btn].just_repeated; }
int Input_isPressed(GamePadIndex btn) { return gamePad[btn].is_pressed; }
int Input_justReleased(GamePadIndex btn) { return gamePad[btn].just_released; }
///////////////////////////////////////
// Utilities functions



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
// Debug logging
void LOG_note(int level, const char *fmt, ...)
{
	char buf[1024] = {0};
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);
	switch (level)
	{
#ifdef DEBUG
	case LOG_DEBUG:
		printf("[DEBUG] %s", buf);
		break;
#endif
	case LOG_INFO:
		printf("[INFO] %s", buf);
		break;
	case LOG_WARN:
		fprintf(stderr, "[WARN] %s", buf);
		break;
	case LOG_ERROR:
		fprintf(stderr, "[ERROR] %s", buf);
		break;
	default:
		break;
	}
	fflush(stdout);
}
///////////////////////////////////////////////////////////////
// Video screen surface
static SDL_Surface *screen;
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

static int can_poweroff = 1;
void disablePoweroff(void)
{
	can_poweroff = 0;
}
void waitForWake(void)
{
	SDL_Event event;
	int wake = 0;
	unsigned long sleep_ticks = SDL_GetTicks();
	while (!wake)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_KEYUP)
			{
				SDLKey key = event.key.keysym.sym;
				if (key == CODE_POWER)
				{
					wake = 1;
					break;
				}
			}
		}
		SDL_Delay(200);
		if (can_poweroff && SDL_GetTicks() - sleep_ticks >= 120000)
		{ // increased to two minutes
			if (isCharging())
				sleep_ticks += 60000; // check again in a minute
			else
				powerOff();
		}
	}
	return;
}

void fauxSleep(void)
{
	GFX_clear();
	Input_reset();

	enterSleep();
	system("killall -s STOP keymon");
	waitForWake();
	system("killall -s CONT keymon");
	exitSleep();
}

int isCharging(void)
{
	// Code adapted from OnionOS
	char *cmd = "cd /customer/app/ ; ./axp_test";
	int batJsonSize = 100;
	char buf[batJsonSize];
	int charge_number;
	int result;

	FILE *fp;
	fp = popen(cmd, "r");
	if (fgets(buf, batJsonSize, fp) != NULL)
	{
		sscanf(buf, "{\"battery\":%*d, \"voltage\":%*d, \"charging\":%d}", &charge_number);
		result = (charge_number == 3);
	}
	pclose(fp);
	return result;
}

#define GOVERNOR_PATH "/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor"
static char governor[128];
void enterSleep(void)
{
	SetMute(1);
	putInt("/sys/class/gpio/export", 4);
	putFile("/sys/class/gpio/gpio4/direction", "out");
	putInt("/sys/class/gpio/gpio4/value", 0);

	// save current governor (either ondemand or performance)
	getFile(GOVERNOR_PATH, governor, 128);
	trimTrailingNewlines(governor);

	putFile(GOVERNOR_PATH, "powersave");
	sync();
}

void exitSleep(void)
{
	putInt("/sys/class/gpio/gpio4/value", 1);
	putInt("/sys/class/gpio/unexport", 4);
	putInt("/sys/class/pwm/pwmchip0/export", 0);
	putInt("/sys/class/pwm/pwmchip0/pwm0/enable", 0);
	putInt("/sys/class/pwm/pwmchip0/pwm0/enable", 1);
	SetMute(0);
	SetVolume(GetVolume());
	// restore previous governor
	putFile(GOVERNOR_PATH, governor);
}

int preventAutosleep(void)
{
	return isCharging();
}

void powerOff(void)
{
	if (can_poweroff)
	{
		char *msg = exists(AUTO_RESUME_PATH) ? "Quicksave created,\npowering off" : "Powering off";
		SDL_FillRect(screen, NULL, 0);
		GFX_blitParagraph(screen, msg, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		SDL_Flip(screen);
		sleep(1);
		system("shutdown");
		while (1)
			pause();
	}
}