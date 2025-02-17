#ifndef _CLOCK_H
#define _CLOCK_H

/* CLOCK */

#define MAX_YEAR 2100
#define MIN_YEAR 1970
#define DEFAULT_YEAR 2022
#define MAX_DAY 31
#define MIN_DAY 1
#define MAX_MONTH 12
#define MIN_MONTH 1
#define MAX_SECOND 59
#define MAX_MINUTE MAX_SECOND
#define MAX_HOUR 23
#define FRAME_DURATION 17

enum {
	CURSOR_YEAR,
	CURSOR_MONTH,
	CURSOR_DAY,
	CURSOR_HOUR,
	CURSOR_MINUTE,
	CURSOR_SECOND,
	CURSOR_AMPM,
};


#endif