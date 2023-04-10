#ifndef menu_h__
#define menu_h__

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

typedef MenuReturnStatus (*ShowMenu_t)(void);
MenuReturnStatus ShowMenu(void);

#endif
