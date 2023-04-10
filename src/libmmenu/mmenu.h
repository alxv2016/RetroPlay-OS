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
	char dir_name[MAX_PATH];
	char bmp_path[MAX_PATH];
	int rom_disc;
	int disc;
	int total_discs;
} Rom;

typedef struct MenuSettings {
	int save_exists;
	int preview_exists;
	int quit;
	int dirty;
	int show_setting; // 1=brightness,2=volume
	int setting_value;
	int setting_min;
	int setting_max;
	int simple_mode;
} Menu;

//NOTE: something for image compression?
typedef struct __attribute__((__packed__)) uint24_t {
	uint8_t a,b,c;
} uint24_t;
typedef void (*AutoSave_t)(void);
////////////////////////////////////////////////////////////
void menu_init(void);
void assets_init(void);
void menu_quit(void);
SDL_Surface* slot_thumbnail(SDL_Surface* src_img);
void create_thumbnail(Rom game, int status, SDL_Surface* optional_snapshot);
void show_warning(void);
void load_game(char* rom_path, Rom game, int show_menu);
void input_events(Rom game, int selected, int status, SDL_Surface* optional_snapshot);
int SaveLoad(char* rom_path, char* save_path_template, SDL_Surface* optional_snapshot, int requested_state, AutoSave_t autosave);
int ShowMenu(char* rom_path, char* save_path_template, SDL_Surface* optional_snapshot, int requested_state, AutoSave_t autosave);
void SystemRequest(char* rom_path, int request, AutoSave_t autosave);
int ResumeSlot(void);
int ChangeDisc(char* disc_path);
void gfx_menu(SDL_Surface* cache, int selected);

#endif
