#ifndef _SETTINGS_
#define _SETTINGS_

#define MENU_ITEMS 5
#define TIMER_ITEMS 4

typedef enum SettingsMenu {
	SETTINGS_SLEEP,
	SETTINGS_POWER,
	SETTINGS_SLEEPTIME,
	SETTINGS_SCREEN,
	SETTINGS_VOLUMN
} SettingsMenu;

typedef enum SleepTimes {
	SETTINGS_5,
	SETTINGS_15,
	SETTINGS_25,
	SETTINGS_30
} SleepTimes;

// Icons
typedef enum UtilIcons {
	VOLUME_ICON,
	BRIGHTNESS_ICON,
	MUTE_ICON
} UtilIcons;

extern char *menuItems[MENU_ITEMS];
extern int timerItems[TIMER_ITEMS];

void initSettings(SDL_Surface *surface, int selected, int volValue, int britValue, int sleepValue);
void setSleepTime(int value);
int getSleepTime(void);

#endif