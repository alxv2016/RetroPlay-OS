#ifndef menu_h__
#define menu_h__

typedef enum MenuReturnStatus {
	STATUS_CONTINUE = 0,
	STATUS_SAVESLOT = 1,
	STATUS_LOADSLOT = 11,
	STATUS_OPENMENU = 23,
	STATUS_CHANGEDISC = 24,
	STATUS_RESET = 25,
	STATUS_EXIT = 30,
	STATUS_POWER = 31,
} MenuReturnStatus;

typedef enum MenuRequestState {
	REQUEST_MENU,
	REQUEST_SLEEP,
	REQUEST_POWER,
	REQUEST_SAVE,
	REQUEST_LOAD,
} MenuRequestState;

typedef void (*AutoSave_t)(void);
typedef struct SDL_Surface SDL_Surface;
typedef MenuReturnStatus (*ShowMenu_t)(char* rom_path, char* save_path_template, SDL_Surface* optional_snapshot, MenuRequestState requested_state, AutoSave_t autosave);
MenuReturnStatus ShowMenu(char* rom_path, char* save_path_template, SDL_Surface* optional_snapshot, MenuRequestState request_state, AutoSave_t autosave);

typedef void (*ShowWarning_t)(void);
void ShowWarning(void);

typedef int (*ResumeSlot_t)(void);
int ResumeSlot(void);

typedef int (*ChangeDisc_t)(char* disc_path);
int ChangeDisc(char* disc_path);

#endif  // menu_h__
