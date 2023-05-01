#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <SDL/SDL_mixer.h>
#include <msettings.h>

#include "defines.h"
#include "rumble.h"

/* RUMBLE */

static Mix_Chunk *arrowSound = NULL;
static Mix_Chunk *clickSound = NULL;
static int super_short_timings[] = {0, 25, 50, 75};
static int short_timings[] = {0, 50, 100, 150};
static int msleep_interrupt = 0;
static int vibration_timing = 2;

static int fileWrite(const char *path, const char *str, uint32_t len) {
  uint32_t fd;
  if ((fd = open(path, O_WRONLY)) == 0)
    return 1;
  if (write(fd, str, len) == -1)
    return 1;
  close(fd);
  return 0;
}

/* msleep(): Sleep for the requested number of milliseconds. */
static int msleep(long msec) {
  struct timespec ts;
  int res;

  if (msec < 0) {
    errno = EINVAL;
    return -1;
  }

  ts.tv_sec = msec / 1000;
  ts.tv_nsec = (msec % 1000) * 1000000;

  do {
    res = nanosleep(&ts, &ts);
  } while (res && errno == EINTR && msleep_interrupt == 0);

  msleep_interrupt = 0;
  return res;
}

// Rumble port from ONION OS
void rumble(int enabled) {
  fileWrite("/sys/class/gpio/export", "48", 2);
  fileWrite("/sys/class/gpio/gpio48/direction", "out", 3);
  fileWrite("/sys/class/gpio/gpio48/value", enabled ? "0" : "1", 1);
}

// Turns on vibration for 100ms
void shortPulse(void) {
  rumble(ON);
  msleep(short_timings[vibration_timing]);
  rumble(OFF);
}

// Turns on vibration for 50ms
void superShortPulse(void) {
  rumble(ON);
  msleep(super_short_timings[vibration_timing]);
  rumble(OFF);
}

// Turns on vibration for 50ms
void menuShortPulse(void) {
  rumble(ON);
  msleep(short_timings[vibration_timing]);
  rumble(OFF);
}

// Turns on vibration for 50ms
void menuSuperShortPulse(void) {
  rumble(ON);
  msleep(super_short_timings[vibration_timing]);
  rumble(OFF);
}

void playClickSound(void) {
    if (Mix_OpenAudio(48000, 32784, 1, 4096) < 0) return;
    char soundPath[512];
    snprintf(soundPath, 512 * 2 - 1, "%s", SOUND_CLICKS);
    int volume = GetVolume() > 0? SOUND_VOL: 0;
    if (clickSound == NULL) clickSound = Mix_LoadWAV(soundPath);
    Mix_Volume(-1, 20);
    Mix_PlayChannel(-1, clickSound, 0);
}

void playArrowSound(void) {
    if (Mix_OpenAudio(48000, 32784, 1, 4096) < 0) return;
    char soundPath[512];
    snprintf(soundPath, 512 * 2 - 1, "%s", SOUND_ARROWS);
    int volume = GetVolume() > 0? SOUND_VOL: 0;
    if (arrowSound == NULL) arrowSound = Mix_LoadWAV(soundPath);
    Mix_Volume(-1, volume);
    Mix_PlayChannel(-1, arrowSound, 0);
}

void freeSound() {
  Mix_FreeChunk(clickSound);
  Mix_FreeChunk(arrowSound);
}