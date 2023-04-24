// miyoomini/keymon.c

#include <dirent.h>
#include <fcntl.h>
#include <linux/input.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <SDL/SDL_mixer.h>
#include <msettings.h>

#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../common/keycontext.h"

#include "keymon.h"

// for DEBUG
// #define	DEBUG
#ifdef DEBUG
#define ERROR(str)                                                             \
  fprintf(stderr, str "\n");                                                   \
  quit(EXIT_FAILURE)
#else
#define ERROR(str) quit(EXIT_FAILURE)
#endif

static SAR_ADC_CONFIG_READ adc_config = {0, 0};
static int is_charging = 0;
static int eased_charge = 0;
static int sar_fd = 0;
static int input_fd = 0;
static pthread_t adc_pt;

void quit(int exitcode) {
  pthread_cancel(adc_pt);
  pthread_join(adc_pt, NULL);
  QuitSettings();

  if (input_fd > 0)
    close(input_fd);
  if (sar_fd > 0)
    close(sar_fd);
  exit(exitcode);
}

void checkAXP(void) {
  // Code adapted from OnionOS
  char *cmd = "cd /customer/app/ ; ./axp_test";
  int batJsonSize = 100;
  char buf[batJsonSize];
  int battery_number;

  FILE *fp;
  fp = popen(cmd, "r");
  if (fgets(buf, batJsonSize, fp) != NULL) {
    sscanf(buf, "{\"battery\":%d, \"voltage\":%*d, \"charging\":%*d}",
           &battery_number);
  }
  pclose(fp);

  int bat_fd = open("/tmp/battery", O_CREAT | O_WRONLY | O_TRUNC);
  if (bat_fd > 0) {
    char value[3];
    sprintf(value, "%d", battery_number);
    write(bat_fd, value, strlen(value));
    close(bat_fd);
  }
}

static void *runAXP(void *arg) {
  while (1) {
    sleep(5);
    checkAXP();
  }
  return 0;
}

int main(int argc, char *argv[]) {
  checkAXP();
  pthread_create(&adc_pt, NULL, &runAXP, NULL);
  // Set Initial Volume / Brightness
  InitSettings();

  input_fd = open("/dev/input/event0", O_RDONLY);
  // Main Loop
  register uint32_t val;
  register uint32_t button_flag = 0;
  register uint32_t menu_pressed = 0;
  register uint32_t power_pressed = 0;
  uint32_t repeat_vol = 0;

  while (read(input_fd, &ev, sizeof(ev)) == sizeof(ev)) {
    val = ev.value;
    if ((ev.type != EV_KEY) || (val > REPEAT))
      continue;
    switch (ev.code) {
    case BUTTON_MENU:
      if (val != REPEAT)
        menu_pressed = val;
      if (val == PRESSED)
      break;
    case BUTTON_POWER:
      if (val != REPEAT)
        power_pressed = val;
      if (val == PRESSED)
        break;
    case BUTTON_START:
      if (val != REPEAT) {
        button_flag = button_flag & (~START) | (val << START_BIT);
      }
      if (val == PRESSED)
      break;
    case BUTTON_SELECT:
      if (val == PRESSED)
      break;
    case BUTTON_VOLUP:
      if (val == REPEAT) {
        // Adjust repeat speed to 1/2
        val = repeat_vol;
        repeat_vol ^= PRESSED;
      } else {
        repeat_vol = 0;
      }
      if (val == PRESSED) {
        val = GetVolume();
        if (val < MAX_VOLUME)
          SetVolume(++val);
        if (val > 0)
          SetMute(0);
        // if (menu_pressed > 0) {
        //   val = GetBrightness();
        //   if (val < MAX_BRIGHTNESS)
        //     SetBrightness(++val);
        // } else {
        //   val = GetVolume();
        //   if (val < MAX_VOLUME)
        //     SetVolume(++val);
        //   if (val > 0)
        //     SetMute(0);
        // }
      }
      break;
    case BUTTON_VOLDOWN:
      if (val == REPEAT) {
        // Adjust repeat speed to 1/2
        val = repeat_vol;
        repeat_vol ^= PRESSED;
      } else {
        repeat_vol = 0;
      }
      if (val == PRESSED) {
        val = GetVolume();
        if (val > 0)
          SetVolume(--val);
        if (val == 0)
          SetMute(1);
        // if (menu_pressed > 0) {
        //   val = GetBrightness();
        //   if (val > 0)
        //     SetBrightness(--val);
        // } else {
        //   val = GetVolume();
        //   if (val > 0)
        //     SetVolume(--val);
        //   if (val == 0)
        //     SetMute(1);
        // }
      }
      break;
    case BUTTON_UP:
      if (val == PRESSED)
      break;
    case BUTTON_DOWN:
      if (val == PRESSED)
      break;
    case BUTTON_LEFT:
      if (val == PRESSED)
      break;
    case BUTTON_RIGHT:
      if (val == PRESSED)
      break;
    case BUTTON_A:
      if (val == PRESSED)
      break;
    case BUTTON_B:
      if (val == PRESSED)
      break;
    case BUTTON_X:
      if (val == PRESSED)
      break;
    case BUTTON_Y:
      if (val == PRESSED)
      break;
    default:
      break;
    }

    if (menu_pressed && power_pressed) {
      menu_pressed = power_pressed = 0;
      system("shutdown");
      while (1)
        pause();
    }
  }
  ERROR("Failed to read input event");
}
