#ifndef _RUMBLE_H
#define _RUMBLE_H

/* RUMBLE */
enum RumbleState {
  OFF,
  ON,
};

void rumble(int enabled);
void shortPulse(void);
void superShortPulse(void);
void menuShortPulse(void);
void menuSuperShortPulse(void);

void freeSound(void);
void playClick(void);
#endif