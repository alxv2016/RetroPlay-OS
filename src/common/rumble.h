#ifndef RUMBLE_H__
#define RUMBLE_H__

#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

#define SHORT_PULSE_MS 100
#define SUPER_SHORT_PULSE_MS 50

static int super_short_timings[] = {0, 25, 50, 75};
static int short_timings[] = {0, 50, 100, 150};
static int msleep_interrupt = 0;
static int vibration_timing = 2;

// Rumble port from ONION OS
void rumble(int enabled)
{
    file_write("/sys/class/gpio/export", "48", 2);
    file_write("/sys/class/gpio/gpio48/direction", "out", 3);
    file_write("/sys/class/gpio/gpio48/value", enabled ? "0" : "1", 1);
}

// Turns on vibration for 100ms
void short_pulse(void)
{
    rumble(true);
    msleep(short_timings[vibration_timing]);
    rumble(false);
}

// Turns on vibration for 50ms
void super_short_pulse(void)
{
    rumble(true);
    msleep(super_short_timings[vibration_timing]);
    rumble(false);
}

// Turns on vibration for 50ms
void menu_short_pulse(void)
{
    rumble(true);
    msleep(short_timings[vibration_timing]);
    rumble(false);
}

// Turns on vibration for 50ms
void menu_super_short_pulse(void)
{
    rumble(true);
    msleep(super_short_timings[vibration_timing]);
    rumble(false);
}

/* msleep(): Sleep for the requested number of milliseconds. */
int msleep(long msec)
{
    struct timespec ts;
    int res;

    if (msec < 0) {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR && msleep_interrupt == 0);

    msleep_interrupt = 0;

    return res;
}

int file_write(const char *path, const char *str, uint32_t len)
{
    uint32_t fd;
    if ((fd = open(path, O_WRONLY)) == 0)
        return 1;
    if (write(fd, str, len) == -1)
        return 1;
    close(fd);
    return 0;
}

#endif 
