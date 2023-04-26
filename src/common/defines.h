#ifndef _DEFINES_H
#define _DEFINES_H

// Raw Hex Colors
#define TRIAD_WHITE 0xff, 0xff, 0xff
#define TRIAD_BLACK 0x00, 0x00, 0x00
#define TRIAD_LIGHT_GRAY 0x7f, 0x7f, 0x7f
#define TRIAD_GRAY 0x99, 0x99, 0x99
#define TRIAD_DARK_GRAY 0x26, 0x26, 0x26
#define TRIAD_LIGHT_TEXT 0xcc, 0xcc, 0xcc
#define TRIAD_DARK_TEXT 0x00, 0x00, 0x00
#define TRIAD_ACTIVE 0x23, 0x23, 0x23
#define TRIAD_ACCENT 0xDB, 0xFF, 0x4D
#define TRIAD_GRAY200 0x51, 0x51, 0x51
// Colors
#define COLOR_WHITE (SDL_Color){TRIAD_WHITE}
#define COLOR_GRAY (SDL_Color){TRIAD_GRAY}
#define COLOR_BLACK (SDL_Color){TRIAD_BLACK}
#define COLOR_LIGHT_TEXT (SDL_Color){TRIAD_LIGHT_TEXT}
#define COLOR_DARK_TEXT (SDL_Color){TRIAD_DARK_TEXT}
#define COLOR_GRAY_TEXT (SDL_Color){TRIAD_GRAY}
#define COLOR_BUTTON_TEXT (SDL_Color){TRIAD_GRAY}
#define COLOR_ACTIVE (SDL_Color){TRIAD_ACTIVE}
#define COLOR_ACCENT (SDL_Color){TRIAD_ACCENT}
#define COLOR_GRAY200 (SDL_Color){TRIAD_GRAY200}

// Screen size
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

// UI Dimensions
#define ICON_SIZE 28
#define CONSOLE_ICON_SIZE 48
#define BUTTON_SIZE 32
#define RADIUS 16

// Screen spacing
#define PADDING 20
#define PADDING_TOP 12
#define PADDING_LR 20

/// MENU
#define ROW_COUNT 5
#define ROW_HEIGHT 64
#define MAX_SIZE 16
#define MAX_ROW 16

// Font size
#define FONT_LARGE 32
#define FONT_MEDIUM 28
#define FONT_SMALL 24
#define FONT_TINY 22
#define FONT_FOOTNOTE 16
#define FONT_LINEHEIGHT 32

// Paths
#define MAX_PATH 512
#define TEMP_PATH "/tmp/"
#define BATTERY_INFO "/tmp/battery"
#define SOUND_PATH "/customer/app/sound/change.wav"

#define SDCARD_PATH "/mnt/SDCARD"
#define RES_PATH "/mnt/SDCARD/.system/res"
#define PAKS_PATH "/mnt/SDCARD/.system/paks"
#define USERDATA_PATH "/mnt/SDCARD/.userdata"
#define ROMS_PATH "/mnt/SDCARD/Roms"
#define RECENT_PATH "/mnt/SDCARD/.userdata/.retroplay/recent.txt"
#define CONFIG_PATH "/mnt/SDCARD/.userdata/.retroplay/sleeptime.txt"
#define FAUX_RECENT_PATH "/mnt/SDCARD/Recently Played"
#define COLLECTIONS_PATH "/mnt/SDCARD/Collections"
#define FONT_PATH "/mnt/SDCARD/.system/res/Roboto-Regular.ttf"
#define DEBUG_PATH "/mnt/SDCARD/debug.txt"

#define LAST_PATH "/tmp/last.txt"
#define CHANGE_DISC_PATH "/tmp/change_disc.txt"
#define RESUME_SLOT_PATH "/tmp/mmenu_slot.txt"
#define AUTO_RESUME_PATH "/mnt/SDCARD/.userdata/.retroplay/auto_resume.txt"
#define AUTO_RESUME_SLOT "9"

#endif