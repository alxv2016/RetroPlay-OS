#include <SDL/SDL.h>
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <msettings.h>

#include "keycontext.h"
#include "controls.h"

/* CONTROL INPUTS */
static struct GamePadContext gamePad[BTN_COUNT];

void Input_reset(void) {
  for (int i = 0; i < BTN_COUNT; i++) {
    gamePad[i].is_pressed = CODE_NONE;
    gamePad[i].just_pressed = CODE_NONE;
    gamePad[i].just_repeated = CODE_NONE;
    gamePad[i].just_released = CODE_NONE;
  }
}

void Input_poll(void) {
  // reset transient values
  for (int i = 0; i < BTN_COUNT; i++) {
    gamePad[i].just_pressed = CODE_NONE;
    gamePad[i].just_repeated = CODE_NONE;
    gamePad[i].just_released = CODE_NONE;
  }

  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    GamePadIndex i;
    if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
      SDLKey key = event.key.keysym.sym;
      // NOTE: can't use switch because all missing buttons have the same value
      if (key == CODE_A)
        i = BTN_A;
      else if (key == CODE_B)
        i = BTN_B;
      else if (key == CODE_X)
        i = BTN_X;
      else if (key == CODE_Y)
        i = BTN_Y;
      else if (key == CODE_START)
        i = BTN_START;
      else if (key == CODE_SELECT)
        i = BTN_SELECT;
      else if (key == CODE_UP)
        i = BTN_UP;
      else if (key == CODE_DOWN)
        i = BTN_DOWN;
      else if (key == CODE_LEFT)
        i = BTN_LEFT;
      else if (key == CODE_RIGHT)
        i = BTN_RIGHT;
      else if (key == CODE_L1)
        i = BTN_L1;
      else if (key == CODE_R1)
        i = BTN_R1;
      else if (key == CODE_MENU)
        i = BTN_MENU;
      else if (key == CODE_L2)
        i = BTN_L2;
      else if (key == CODE_R2)
        i = BTN_R2;
      else if (key == CODE_PLUS)
        i = BTN_PLUS;
      else if (key == CODE_MINUS)
        i = BTN_MINUS;
      else if (key == CODE_POWER)
        i = BTN_POWER;
      else
        continue;

      if (event.type == SDL_KEYDOWN) {
        gamePad[i].just_repeated = 1;
        if (!gamePad[i].is_pressed) {
          gamePad[i].just_pressed = 1;
          gamePad[i].is_pressed = 1;
        }
      } else {
        gamePad[i].is_pressed = 0;
        gamePad[i].just_released = 1;
      }
    }
  }
}

int Input_anyPressed(void) {
  for (int i = 0; i < BTN_COUNT; i++) {
    if (gamePad[i].is_pressed)
      return 1;
  }
  return 0;
}

int Input_justPressed(GamePadIndex btn) { return gamePad[btn].just_pressed; }
int Input_justRepeated(GamePadIndex btn) { return gamePad[btn].just_repeated; }
int Input_isPressed(GamePadIndex btn) { return gamePad[btn].is_pressed; }
int Input_justReleased(GamePadIndex btn) { return gamePad[btn].just_released; }