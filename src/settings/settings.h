#ifndef _SETTINGS_
#define _SETTINGS_

#define MENU_ITEMS 4

typedef enum SettingsMenu {
	SETTINGS_SLEEP,
	SETTINGS_POWER,
	SETTINGS_SCREEN,
	SETTINGS_VOLUMN,
} SettingsMenu;

// Icons
typedef enum UtilIcons {
	VOLUME_ICON,
	BRIGHTNESS_ICON,
	MUTE_ICON
} UtilIcons;

extern char *menuItems[MENU_ITEMS];

void initSettings(SDL_Surface *surface, int selected, int volValue, int britValue);
void showSomeSettings(SDL_Surface *surface);

#endif