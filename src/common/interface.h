#ifndef _INTERFACE_H
#define _INTERFACE_H

#define FRAME_DURATION 17

typedef struct GFX_Context {
  SDL_Surface *screen;
  SDL_Surface *overlay;
  SDL_Surface *button;
  SDL_Surface *button_outline;
  SDL_Surface *dpad;
  SDL_Surface *dpad2;
  SDL_Surface *button_menu;
  SDL_Surface *corner_radius;
  SDL_Surface *settings_bar_full;
  SDL_Surface *settings_bar_empty;
  SDL_Surface *brightness;
  SDL_Surface *brightness_low;
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
  SDL_Surface *clock;
  SDL_Surface *screenshot;
  SDL_Surface *files;
  SDL_Surface *arcade;
  SDL_Surface *nes;
  SDL_Surface *gameboy;
  SDL_Surface *gamegear;
  SDL_Surface *gba;
  SDL_Surface *gbc;
  SDL_Surface *sega;
  SDL_Surface *snes;
  SDL_Surface *playstation;
  SDL_Surface *empty_state;
  SDL_Surface *empty_screenshots;
  SDL_Surface *poweroff_state;
  SDL_Surface *sleep_state;
} GFX;
// Fonts
typedef struct GFX_Fonts {
  TTF_Font *h1;
  TTF_Font *h2;
  TTF_Font *h3;
  TTF_Font *body;
  TTF_Font *caption;
  TTF_Font *footnote;
  TTF_Font *h1_500;
  TTF_Font *h2_500;
  TTF_Font *h3_500;
  TTF_Font *body_500;
  TTF_Font *caption_500;
  TTF_Font *footnote_500;
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
void GFX_sync(unsigned long frameStart);

void heading(int fontSize, int bold, char* copy, SDL_Color color, SDL_Surface *surface, SDL_Rect *offset);
void paragraph(int fontSize, int bold, char* copy, SDL_Color color, SDL_Surface *surface, SDL_Rect *offset);
int truncateText(TTF_Font *font, char *displayName,int max_width, int padding);
SDL_Surface *loadImage(char *path);
SDL_Surface *renderText(char *text);

void emptyState(SDL_Surface *surface, SDL_Surface *icon, int headingSize, int bodySize, char *headingCopy, char *bodyCopy);
void powerOffState(SDL_Surface *surface, char *headingCopy, char *bodyCopy);
void sleepState(SDL_Surface *surface, char *headingCopy, char *bodyCopy);
void listMenu(SDL_Surface *surface, char *path, int consoleDir, int recentDir, char *emuTag, char *name, char *unique, int row, int selected, int total);
void batteryStatus(SDL_Surface *surface, int x, int y);
void primaryBTN(SDL_Surface *surface, char *bkey, char *blabel, int rightAlign, int x, int y);
void secondaryBTN(SDL_Surface *surface, char *bkey, char *blabel, int rightAlign, int x, int y);
void primaryBTN(SDL_Surface *surface, char *bkey, char *blabel, int rightAlign, int x, int y);
void tertiaryBTN(SDL_Surface *surface, char *blabel, int altUI, int rightAlign, int x, int y);
void volumeControl(SDL_Surface *surface, int x, int y, int bottomAlign, int value, int minValue, int maxValue);
void brightnessControl(SDL_Surface *surface, int x, int y, int value, int minValue, int maxValue);

#endif