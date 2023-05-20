#ifndef menu_h__
#define menu_h__

//NOTES: have to use this naming convention due to dependency from legacy picoarch code for mmenu integration
typedef enum MenuReturnStatus {
  kStatusContinue = 0,
  kStatusSaveSlot = 1,
  kStatusLoadSlot = 11,
  kStatusOpenMenu = 23,
  kStatusChangeDisc = 24,
  kStatusResetGame = 25,
  kStatusExitGame = 30,
  kStatusPowerOff = 31,
} MenuReturnStatus;

typedef enum MenuRequestState {
  kRequestMenu,
  kRequestSleep,
  kRequestPowerOff,
  kRequestSave,
  kRequestLoad,
} MenuRequestState;

typedef void (*AutoSave_t)(void);
typedef struct SDL_Surface SDL_Surface;
typedef void (*ShowWarning_t)(void);
typedef int (*ResumeSlot_t)(void);
typedef int (*ChangeDisc_t)(char* disc_path);

typedef MenuReturnStatus (*ShowMenu_t)(char* rom_path, char* save_path_template, SDL_Surface* optional_snapshot, MenuRequestState requested_state, AutoSave_t autosave);
MenuReturnStatus ShowMenu(char* rom_path, char* save_path_template, SDL_Surface* optional_snapshot, MenuRequestState request_state, AutoSave_t autosave);

void ShowWarning(void);
int ResumeSlot(void);
int ChangeDisc(char* disc_path);

#endif