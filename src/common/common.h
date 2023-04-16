#ifndef COMMON_H
#define COMMON_H

///////////////////////////////////////////
int GFX_truncateText(TTF_Font *font, const char *in_name, char *out_name, int max_width, int padding);

//////////////////////////////////////
// Graphics UI
void GFX_init(void);
void GFX_menuInit(void);
void GFX_quit(void);
void GFX_menuQuit(void);
void GFX_clear(void);
void GFX_ready(void);
SDL_Surface *GFX_loadImage(char *path);

// Components
int GFX_getButtonWidth(char *blabel, char *bkey);
int GFX_scrollLongNames(SDL_Surface *surface, char *name, char *path, char *unique, int row, int selected, int reset, int force);
void GFX_blitButton(SDL_Surface *surface, char *bkey, char *blabel, int x, int y);
void GFX_blitPill(SDL_Surface *surface, char *bkey, char *blabel, int x, int y);
void GFX_blitHint(SDL_Surface *surface, char *htxt, int x, int y);
void GFX_blitMainMenu(SDL_Surface *surface, char *name, char *path, char *unique, int row, int selected_row);
void GFX_blitIngameWindow(SDL_Surface *surface, int x, int y, int width, int height);
void GFX_blitParagraph(SDL_Surface *surface, char *text, int x, int y, int width, int height);
void GFX_blitText(SDL_Surface *surface, char *text, int x, int y, int color);
void GFX_blitBattery(SDL_Surface *surface, int x, int y);
void GFX_blitSettings(SDL_Surface *surface, int x, int y, int icon, int value, int min_value, int max_value); // icon=0:brightness,1:volume,2:mute

SDL_Surface *GFX_getText(char *text);
/////////////////////////////////////////

#endif