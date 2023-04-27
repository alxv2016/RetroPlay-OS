#ifndef _POWEROPS_H
#define _POWEROPS_H

#define SHORT_PULSE_MS 100
#define SUPER_SHORT_PULSE_MS 50
#define CHARGE_DELAY 1000

#define GOVERNOR_PATH "/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor"

extern int can_poweroff;
extern char governor[128];
extern SDL_Surface *screen;

/* POWER */
void disablePoweroff(void);
void waitForWake(void);
void fauxSleep(void);
void enterSleep(void);
void exitSleep(void);
int preventAutosleep(void);
void powerOff(void);
int getSleepDelay(void);

#endif