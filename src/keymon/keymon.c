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
#include "../common/screencapture.h"

#include "keymon.h"

#define ERROR(str) quit(EXIT_FAILURE)

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
  InitSettings();
  pthread_create(&adc_pt, NULL, &runAXP, NULL);

  input_fd = open("/dev/input/event0", O_RDONLY);

  uint32_t val;
  uint32_t button_flag = 0;
  uint32_t menu_pressed = 0;
  uint32_t power_pressed = 0;
  uint32_t l2_pressed = 0;
  uint32_t r2_pressed = 0;
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
      }
      break;
    case BUTTON_L2:
    case BUTTON_R2:
      if ( ev.code == BUTTON_L2 ) {
				l2_pressed = val;
			} else if ( ev.code == BUTTON_R2 ) {
				r2_pressed = val;
			}
			if (l2_pressed & r2_pressed) {
				screenshot();
				usleep(100000);	//0.1s
				l2_pressed = r2_pressed = 0;
			}
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
