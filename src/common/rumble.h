#ifndef _RUMBLE_H
#define _RUMBLE_H

#define SOUND_VOL 15
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
void playArrowSound(void);
void playClickSound(void);
#endif