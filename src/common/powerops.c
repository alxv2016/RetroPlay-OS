#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <msettings.h>

#include "defines.h"
#include "keycontext.h"
#include "utils.h"

#include "interface.h"
#include "controls.h"
#include "rumble.h"

#include "powerops.h"

/* POWER */
int can_poweroff = 1;
char governor[128];
GFX gfx;

void disablePoweroff(void) { can_poweroff = 0; }

void waitForWake(void) {
  SDL_Event event;
  int wake = 0;
  unsigned long sleep_ticks = SDL_GetTicks();
  while (!wake) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_KEYUP) {
        SDLKey key = event.key.keysym.sym;
        if (key == CODE_POWER) {
          wake = 1;
          break;
        }
      }
    }
    SDL_Delay(200);
    if (can_poweroff &&
        SDL_GetTicks() - sleep_ticks >= 120000) { // increased to two minutes
      if (isCharging())
        sleep_ticks += 60000; // check again in a minute
      else
        powerOff();
    }
  }
  return;
}

void fauxSleep(void) {
  menuSuperShortPulse();
  GFX_clear();
  Input_reset();

  enterSleep();
  system("killall -s STOP keymon");
  waitForWake();
  system("killall -s CONT keymon");
  exitSleep();
}

void enterSleep(void) {
  menuSuperShortPulse();
  SetMute(1);
  putInt("/sys/class/gpio/export", 4);
  putFile("/sys/class/gpio/gpio4/direction", "out");
  putInt("/sys/class/gpio/gpio4/value", 0);

  // save current governor (either ondemand or performance)
  getFile(GOVERNOR_PATH, governor, 128);
  trimTrailingNewlines(governor);

  putFile(GOVERNOR_PATH, "powersave");
  sync();
}

void exitSleep(void) {
  menuSuperShortPulse();
  putInt("/sys/class/gpio/gpio4/value", 1);
  putInt("/sys/class/gpio/unexport", 4);
  putInt("/sys/class/pwm/pwmchip0/export", 0);
  putInt("/sys/class/pwm/pwmchip0/pwm0/enable", 0);
  putInt("/sys/class/pwm/pwmchip0/pwm0/enable", 1);
  SetMute(0);
  SetVolume(GetVolume());
  // restore previous governor
  putFile(GOVERNOR_PATH, governor);
}

int preventAutosleep(void) { return isCharging(); }

void powerOff(void) {
  if (can_poweroff) {
    menuSuperShortPulse();
    char *msg = exists(AUTO_RESUME_PATH) ? "Quicksave created,\npowering off"
                                         : "Powering off";
    SDL_FillRect(gfx.screen, NULL, 0);
    paragraph(font.h3, msg, 24, gfx.screen, NULL);
    SDL_Flip(gfx.screen);
    sleep(1);
    system("shutdown");
    while (1)
      pause();
  }
}