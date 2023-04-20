#ifndef _SETTINGS_
#define _SETTINGS_

typedef enum SettingsMenu {
	SETTINGS_SLEEP,
	SETTINGS_POWER,
	SETTINGS_SCREEN,
	SETTINGS_VOLUMN,
} SettingsMenu;

void initSettings(SDL_Surface *surface);

#endif