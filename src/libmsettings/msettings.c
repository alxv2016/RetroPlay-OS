#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <errno.h>
#include <math.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <mi_ao.h>

#include <stdint.h>
#include <sys/ioctl.h>

#include "msettings.h"

/* MSETTINGS */

static Settings DefaultSettings = {
    .version = 1,
    .brightness = 2,
		.headphones = 4,
    .speaker = 20,
		.jack = 0,
};

static Settings *settings;

static char SettingsPath[256];
static int shm_fd = -1;
static int is_host = 0;
static int shm_size = sizeof(Settings);

void InitSettings(void) {
  sprintf(SettingsPath, "%s/msettings.bin", getenv("USERDATA_PATH"));
	// see if it exists
  shm_fd = shm_open(SHM_KEY, O_RDWR | O_CREAT | O_EXCL, 0644);
	// already exists
  if (shm_fd == -1 && errno == EEXIST) {
    puts("Settings client");
    shm_fd = shm_open(SHM_KEY, O_RDWR, 0644);
    settings =
        mmap(NULL, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
  } else {
		// host
    puts("Settings host");
    is_host = 1;
    // we created it so set initial size and populate
    ftruncate(shm_fd, shm_size);
    settings =
        mmap(NULL, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    int fd = open(SettingsPath, O_RDONLY);
    if (fd >= 0) {
      read(fd, settings, shm_size);
      // TODO: use settings->version for future proofing
      close(fd);
    } else {
      // load defaults
      memcpy(settings, &DefaultSettings, shm_size);
    }
  }
  printf("brightness: %i\nspeaker: %i\n", settings->brightness,
         settings->speaker);

  MI_AO_Enable(0);
  MI_AO_EnableChn(0, 0);
  SetVolume(GetVolume());
  if (GetVolume() == 0)
    SetMute(1);
  else
    SetMute(0);
  SetBrightness(GetBrightness());
}

void QuitSettings(void) {
  munmap(settings, shm_size);
  if (is_host)
    shm_unlink(SHM_KEY);
}

static inline void SaveSettings(void) {
  int fd = open(SettingsPath, O_CREAT | O_WRONLY, 0644);
  if (fd >= 0) {
    write(fd, settings, shm_size);
    close(fd);
    sync();
  }
}

int GetBrightness(void) {
	// 0-10
  return settings->brightness;
}
void SetBrightness(int value) {
  if (value > 2) {
    SetRawBrightness(pow(value, 2));
  } else {
    SetRawBrightness(3 + (value * 2));
  }
  settings->brightness = value;
  SaveSettings();
}

int GetVolume(void) {
	return settings->jack ? settings->headphones : settings->speaker;
}

void SetVolume(int value) {
  int raw = -60 + value * 3;
  SetRawVolume(raw);
	if (settings->jack) settings->headphones = value;
	else settings->speaker = value;
  SaveSettings();
}

void SetRawBrightness(int val) {
  int fd = open("/sys/class/pwm/pwmchip0/pwm0/duty_cycle", O_WRONLY);
  if (fd >= 0) {
    dprintf(fd, "%d", val);
    close(fd);
  }
}

void SetRawVolume(int val) { MI_AO_SetVolume(0, val); }

void SetMute(int mute) {
  int fd = open("/dev/mi_ao", O_RDWR);
  if (fd >= 0) {
    int buf2[] = {0, mute};
    uint64_t buf1[] = {sizeof(buf2), (uintptr_t)buf2};

    ioctl(fd, MI_AO_SETMUTE, buf1);
    close(fd);
  }
}

int GetJack(void) { return settings->jack; }

void SetJack(int value) {
 	settings->jack = value;
	SetVolume(GetVolume());
}