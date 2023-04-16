#ifndef _BATMON_H
#define _BATMON_H

/* BATMON */
void screenOn(void);
void screenOff(void);
void checkCharging(void);
void *chargingThread(void *arg);
void *inputThread(void *arg);
int main(void);


#endif