#ifndef COMMON_H
#define COMMON_H

enum {
	LOG_DEBUG = 0,
	LOG_INFO,
	LOG_WARN,
	LOG_ERROR,
};

#define LOG_debug(fmt, ...) LOG_note(LOG_DEBUG, fmt, ##__VA_ARGS__)
#define LOG_info(fmt, ...) LOG_note(LOG_INFO, fmt, ##__VA_ARGS__)
#define LOG_warn(fmt, ...) LOG_note(LOG_WARN, fmt, ##__VA_ARGS__)
#define LOG_error(fmt, ...) LOG_note(LOG_ERROR, fmt, ##__VA_ARGS__)
void LOG_note(int level, const char* fmt, ...);

typedef enum GamePadIndex {
	BTN_UP = 0,
	BTN_DOWN,
	BTN_LEFT,
	BTN_RIGHT,
	BTN_A,
	BTN_B,
	BTN_X,
	BTN_Y,
	BTN_START,
	BTN_SELECT,
	BTN_L1,
	BTN_R1,
	BTN_L2,
	BTN_R2,
	BTN_MENU,
	BTN_POWER,
	BTN_PLUS,
	BTN_MINUS,
	BTN_COUNT,
} GamePadIndex;

void Input_reset(void);
void Input_poll(void);
int Input_anyPressed(void);
int Input_justPressed(GamePadIndex btn);
int Input_justRepeated(GamePadIndex btn);
int Input_isPressed(GamePadIndex btn);
int Input_justReleased(GamePadIndex btn);

///////////////////////////////////////////
// Utilities
int prefixMatch(char* pre, char* str);
int suffixMatch(char* suf, char* str);
int exactMatch(char* str1, char* str2);
int hide(char* file_name);
int exists(char* path);
int getInt(char* path);

void getDisplayName(const char* in_name, char* out_name);
void getEmuName(const char* in_name, char* out_name);
void normalizeNewline(char* line);
void trimTrailingNewlines(char* line);
void touch(char* path);
void putFile(char* path, char* contents);
void getFile(char* path, char* buffer, size_t buffer_size);
void putInt(char* path, int value);
int GFX_truncateText(TTF_Font* font, const char* in_name, char* out_name, int max_width, int padding);

//////////////////////////////////////
// Graphics UI
void GFX_init(void);
void GFX_menuInit(void);
void GFX_quit(void);
void GFX_menuQuit(void);
void GFX_clear(void);
void GFX_ready(void);
SDL_Surface* GFX_loadImage(char* path);

// Components
int GFX_getButtonWidth(char* blabel, char* bkey);
int GFX_scrollLongNames(SDL_Surface* surface, char* name, char* path, char* unique, int row, int selected, int reset, int force);
void GFX_blitButton(SDL_Surface* surface, char* bkey, char* blabel, int x, int y);
void GFX_blitPill(SDL_Surface* surface, char* bkey, char* blabel, int x, int y);
void GFX_blitHint(SDL_Surface* surface, char* htxt, int x, int y);
void GFX_blitMainMenu(SDL_Surface* surface, char* name, char* path, char* unique, int row, int selected_row);
void GFX_blitIngameWindow(SDL_Surface* surface, int x, int y, int width, int height); 
void GFX_blitParagraph(SDL_Surface* surface, char* text, int x, int y, int width, int height); 
void GFX_blitText(SDL_Surface* surface, char* text, int x, int y, int color); 
void GFX_blitBattery(SDL_Surface* surface, int x, int y);
void GFX_blitSettings(SDL_Surface* surface, int x, int y, int icon, int value, int min_value, int max_value); // icon=0:brightness,1:volume,2:mute

SDL_Surface* GFX_getText(char* text);
/////////////////////////////////////////
// POWER Management
void waitForWake(void);
void fauxSleep(void);
void powerOff(void);
void enterSleep(void);
void exitSleep(void);
void disablePoweroff(void);

int isCharging(void);
int preventAutosleep(void);

#endif