#ifndef _MMENU_H
#define _MMENU_H

typedef enum MenuReturnStatus {
	STATUS_CONTINUE = 0,
	STATUS_SAVE = 1,
	STATUS_LOAD = 11,
	STATUS_OPTIONS = 23,
	STATUS_CHANGEDISC = 24,
	STATUS_RESET = 25,
	STATUS_EXIT = 30,
	STATUS_POWER = 31,
} MenuReturnStatus;

typedef enum MenuRequestState {
	REQ_MENU,
	REQ_SLEEP,
	REQ_POWER,
	REQ_SAVE,
	REQ_LOAD,
} MenuRequestState;

typedef void (*AutoSave_t)(void);
typedef struct SDL_Surface SDL_Surface;
typedef void (*ShowWarning_t)(void);
typedef int (*ResumeSlot_t)(void);
typedef int (*ChangeDisc_t)(char* disc_path);

typedef MenuReturnStatus (*ShowMenu_t)(char* rom_path, char* save_path_template, SDL_Surface* optional_snapshot, MenuRequestState requested_state, AutoSave_t autosave);
MenuReturnStatus ShowMenu(char* rom_path, char* save_path_template, SDL_Surface* optional_snapshot, MenuRequestState request_state, AutoSave_t autosave);

// void ShowWarning(void);
// int ResumeSlot(void);
// int ChangeDisc(char* disc_path);

#endif