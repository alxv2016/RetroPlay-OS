#ifndef DEFINES_H
#define DEFINES_H

#define CODE_NONE 0
#define CODE_UP SDLK_UP
#define CODE_DOWN SDLK_DOWN
#define CODE_LEFT SDLK_LEFT
#define CODE_RIGHT SDLK_RIGHT
#define CODE_A SDLK_SPACE
#define CODE_B SDLK_LCTRL
#define CODE_X SDLK_LSHIFT
#define CODE_Y SDLK_LALT
#define CODE_START SDLK_RETURN
#define CODE_SELECT SDLK_RCTRL
#define CODE_MENU SDLK_ESCAPE
#define CODE_POWER SDLK_POWER
#define CODE_L1 SDLK_e
#define CODE_L2 SDLK_TAB
#define CODE_R1 SDLK_t
#define CODE_R2 SDLK_BACKSPACE
#define	CODE_PLUS SDLK_LSUPER
#define	CODE_MINUS SDLK_RSUPER

// Volume and Brightness
#define MIN_VOLUME 0
#define MAX_VOLUME 20
#define MIN_BRIGHTNESS 0
#define MAX_BRIGHTNESS 10

// Raw Hex Colors
#define TRIAD_WHITE 0xff,0xff,0xff
#define TRIAD_BLACK 0x00,0x00,0x00
#define TRIAD_LIGHT_GRAY 0x7f,0x7f,0x7f
#define TRIAD_GRAY 0x99,0x99,0x99
#define TRIAD_DARK_GRAY 0x26,0x26,0x26
#define TRIAD_LIGHT_TEXT 0xcc,0xcc,0xcc
#define TRIAD_DARK_TEXT 0x00,0x00,0x00
// Colors
#define COLOR_WHITE (SDL_Color){TRIAD_WHITE}
#define COLOR_GRAY (SDL_Color){TRIAD_GRAY}
#define COLOR_BLACK (SDL_Color){TRIAD_BLACK}
#define COLOR_LIGHT_TEXT (SDL_Color){TRIAD_LIGHT_TEXT}
#define COLOR_DARK_TEXT (SDL_Color){TRIAD_DARK_TEXT}
#define COLOR_GRAY_TEXT (SDL_Color){TRIAD_GRAY}
#define COLOR_BUTTON_TEXT(SDL_Color){TRIAD_GRAY}

// Screen size
#define BASE_WIDTH 320
#define BASE_HEIGHT 240

#define SCREEN_WIDTH 	640
#define SCREEN_HEIGHT 480
#define SCREEN_SCALE 	2 // SCREEN_HEIGHT / BASE_HEIGHT

// UI Dimensions
#define ICON_SIZE 44
#define PILL_SIZE 44
#define BUTTON_SIZE 44
#define BATTERY_ICON_SIZE 44
#define BUTTON_MARGIN 8
#define BUTTON_PADDING 12
#define SETTINGS_SIZE 4
#define SETTINGS_WIDTH 80
#define ROW_COUNT 6
#define ROW_HEIGHT 64
#define MAX_ROW 16
#define PADDING 20
#define RADIUS 16

// Screen spacing
#define PADDING_TOP 12
#define PADDING_LR 20

// Icons
#define BRIGHTNESS_ICON 0
#define VOLUME_ICON 1
#define VOLUME_MUTE_ICON 2

// Font size
#define FONT_LARGE 32
#define FONT_MEDIUM 28
#define FONT_SMALL 24
#define FONT_TINY 18
#define FONT_LINEHEIGHT 32

// Paths
#define MAX_PATH 512
#define TEMP_PATH "/tmp/"
#define TEMP_BATTERY "/tmp/battery"

#define SDCARD_PATH "/mnt/SDCARD"
#define RES_PATH "/mnt/SDCARD/.system/res"
#define PAKS_PATH "/mnt/SDCARD/.system/paks"
#define USERDATA_PATH "/mnt/SDCARD/.userdata"
#define ROMS_PATH "/mnt/SDCARD/Roms"
#define RECENT_PATH "/mnt/SDCARD/.userdata/.miniui/recent.txt"
#define FAUX_RECENT_PATH "/mnt/SDCARD/Recently Played"
#define COLLECTIONS_PATH "/mnt/SDCARD/Collections"
#define FONT_PATH "/mnt/SDCARD/.system/res/OpenSans-SemiBold.ttf"

#define LAST_PATH "/tmp/last.txt" // transient
#define CHANGE_DISC_PATH "/tmp/change_disc.txt"
#define RESUME_SLOT_PATH "/tmp/mmenu_slot.txt"
#define AUTO_RESUME_PATH "/mnt/SDCARD/.userdata/.miniui/auto_resume.txt"
#define AUTO_RESUME_SLOT "9"
#define SIMPLE_MODE_PATH "/mnt/SDCARD/.userdata/enable-simple-mode"

#define SCALE1(a) ((a)*SCREEN_SCALE)
#define SCALE2(a,b) ((a)*SCREEN_SCALE),((b)*SCREEN_SCALE)
#define SCALE4(a,b,c,d) ((a)*SCREEN_SCALE),((b)*SCREEN_SCALE),((c)*SCREEN_SCALE),((d)*SCREEN_SCALE)

#endif