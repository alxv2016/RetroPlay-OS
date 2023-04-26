#ifndef _DEFINES_H
#define _DEFINES_H

/* STYLE GUIDE */

// Typography 
#define H1 31
#define H2 28
#define H3 25
#define BODY 22
#define CAPTION 20
#define FOOTNOTE 17
#define HEADING_LINEHEIGHT 1.25
#define BODY_LINEHEIGHT 1.5

// Nuetral Colors
#define WHITE 0xff, 0xff, 0xff
#define BLACK 0x00, 0x00, 0x00
#define GREY100 0xe8, 0xe8, 0xe8
#define GREY200 0xb5, 0xb5, 0xb5
#define GREY300 0x81, 0x81, 0x81
#define GREY400 0x4e, 0x4e, 0x4e
#define GREY500 0x1a, 0x1a, 0x1a

// Brand Colors
#define PRIMARY 0xdb, 0xff, 0x4d
#define SECONDARY 0x23, 0xff, 0xe5
#define PRIMARY100 0xf4, 0xff, 0xca
#define PRIMARY200 0xe8, 0xff, 0x8b
#define SECONDARY100 0xbd, 0xff, 0xf7
#define SECONDARY200 0x70, 0xff, 0xee

// Color tokens
#define SURFACE_LIGHT WHITE
#define SURFACE_DARK BLACK
#define SURFACE_ACTIVE GREY500
#define PRIMARY_ACCENT PRIMARY
#define SECONDARY_ACCENT SECONDARY
#define LIGHT_TEXT GREY100
#define DARK_TEXT GREY500
#define NEUTRAL_TEXT GREY300

// Spacing scale
#define SPACING_XS 8
#define SPACING_SM 12
#define SPACING_MD 16
#define SPACING_LG 24
#define SPACING_XL 32
#define SPACING_XXL 48

// Viewport size
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

// Component sizes
#define ICON_SIZE 28
#define CONSOLE_ICON_SIZE 48
#define BUTTON_SIZE 32
#define RADIUS 16

/// List elements
#define ROW_COUNT 5
#define ROW_HEIGHT 64
#define MAX_SIZE 16
#define MAX_ROW 16

// System paths
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
#define FONT_PATH_MEDIUM "/mnt/SDCARD/.system/res/Roboto-Medium.ttf"
#define DEBUG_PATH "/mnt/SDCARD/debug.txt"

#define LAST_PATH "/tmp/last.txt"
#define CHANGE_DISC_PATH "/tmp/change_disc.txt"
#define RESUME_SLOT_PATH "/tmp/mmenu_slot.txt"
#define AUTO_RESUME_PATH "/mnt/SDCARD/.userdata/.retroplay/auto_resume.txt"
#define AUTO_RESUME_SLOT "9"

#endif