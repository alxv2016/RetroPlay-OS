#ifndef _INTERFACE_H
#define _INTERFACE_H

#define FRAME_DURATION 17

typedef struct GFX_Context {
  SDL_Surface *screen;
  SDL_Surface *overlay;
  SDL_Surface *button;
  SDL_Surface *button_outline;
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
  SDL_Surface *digits;
  SDL_Surface *power;
  SDL_Surface *sleep;
  SDL_Surface *sleep_timer;
  SDL_Surface *recents;
  SDL_Surface *apps;
  SDL_Surface *arcade;
  SDL_Surface *nes;
  SDL_Surface *gameboy;
  SDL_Surface *gba;
  SDL_Surface *gbc;
  SDL_Surface *sega;
  SDL_Surface *snes;
  SDL_Surface *playstation;
  SDL_Surface *sys_arcade;
  SDL_Surface *sys_gb;
  SDL_Surface *sys_gba;
  SDL_Surface *sys_gbc;
  SDL_Surface *sys_gg;
  SDL_Surface *sys_nes;
  SDL_Surface *sys_playstation;
  SDL_Surface *sys_sega;
  SDL_Surface *sys_snes;
  SDL_Surface *empty_state;
} GFX;
// Fonts
typedef struct GFX_Fonts {
  TTF_Font *h1;
  TTF_Font *h2;
  TTF_Font *h3;
  TTF_Font *body;
  TTF_Font *caption;
  TTF_Font *footnote;
} Font;

enum Console {
  ARCADE,
  GAMEBOY,
  GBA,
  GBC,
  NES,
  PS,
  SEGA,
  SNES,
};

extern GFX gfx;
extern Font font;

void GFX_init(void);
void GFX_clear(void);
void GFX_ready(void);
void GFX_quit(void);
void GFX_sync(unsigned long frameStart );

void emptyState2(SDL_Surface *surface, TTF_Font *heading, TTF_Font *body, char *headingCopy, char *bodyCopy);

void emptyState(SDL_Surface *surface, TTF_Font *font, int lineHeight, char *msg);
void listMenu(SDL_Surface *surface, char *path, int consoleDir, char *emuTag, char *name, char *unique, int row, int selected);
void batteryStatus(SDL_Surface *surface, int x, int y);
void button(SDL_Surface *surface, char *bkey, char *blabel, int outline, int rightAlign, int x, int y);
void pillButton(SDL_Surface *surface, char *bkey, char *blabel, int x, int y);
void volumnBrightness(SDL_Surface *surface, int x, int y,int icon, int value, int minValue,int maxValue);
void hintLabel(SDL_Surface *surface, char *htxt, int x, int y);
void paragraph(TTF_Font* font, char* msg, int lineHeight, SDL_Surface* surface, SDL_Rect* dst_rect);
void inlineText(SDL_Surface *surface, char *str, int x, int y, int dark);
int getButtonWidth(char *blabel);
int truncateText(TTF_Font *font, char *displayName,int max_width, int padding);
SDL_Surface *loadImage(char *path);
SDL_Surface *renderText(char *text);

#endif