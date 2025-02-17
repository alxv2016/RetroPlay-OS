#ifndef _MSETTINGS_H
#define _MSETTINGS_H

#define SHM_KEY "/SharedSettings"
#define MI_AO_SETMUTE 0x4008690d

typedef struct Settings
{
	int version; // future proofing
	int brightness;
  int headphones;
	int unused1;
	int speaker;
	int unused[4]; // for future use
  int jack;
} Settings;

void InitSettings(void);
void QuitSettings(void);

int GetBrightness(void);
int GetVolume(void);

void SetRawBrightness(int value); // 0-100
void SetRawVolume(int value);     // -60-0
void SetMute(int mute);           // 0-1

void SetBrightness(int value); // 0-10
void SetVolume(int value);     // 0-20

int GetJack(void);
void SetJack(int value); // 0-1

#endif
