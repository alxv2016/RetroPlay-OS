#ifndef _CONTROLS_H
#define _CONTROLS_H

struct GamePadContext
{
	int is_pressed;
	int just_pressed;
	int just_repeated;
	int just_released;
};

typedef enum GamePadIndex
{
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

#endif