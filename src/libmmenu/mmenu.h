#ifndef menu_h__
#define menu_h__

///////////////////////////////////////
#define MAX_ITEMS 5
#define MAX_SLOTS 8
#define CHARGE_DELAY 1000
#define SLEEP_DELAY 30000
#define FRAME_DELAY 17

typedef enum MenuItems {
	ITEM_CONTINUE,
	ITEM_SAVE,
	ITEM_LOAD,
	ITEM_OPTIONS,
	ITEM_QUIT,
} MenuItems;

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

typedef struct Game {
	char rom_file[MAX_PATH]; // with extension
	char rom_name[MAX_PATH]; // without extension or cruft
	char slot_path[MAX_PATH];
	char emu_name[MAX_PATH];
	char mmenu_dir[MAX_PATH];
	char txt_path[MAX_PATH];
	char base_path[MAX_PATH];
	char disc_name[MAX_PATH];
	char m3u_path[MAX_PATH];
	char base_path[MAX_PATH];
	char dir_name[MAX_PATH];
	char bmp_path[MAX_PATH];
	int rom_disc = -1;
	int disc;
	int total_discs;
} Rom;

struct MenuSettings {
	int save_exists = 0;
	int preview_exists = 0;
	int quit = 0;
	int dirty = 1;
	int show_setting = 0; // 1=brightness,2=volume
	int setting_value = 0;
	int setting_min = 0;
	int setting_max = 0;
	int simple_mode = 0;
} menu;

//NOTE: something for image compression?
typedef struct __attribute__((__packed__)) uint24_t {
	uint8_t a,b,c;
} uint24_t;




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
