.PHONY: all clean

ifeq (,$(CROSS_COMPILE))
$(error missing CROSS_COMPILE for this toolchain)
endif

TARGET = clock

CC = $(CROSS_COMPILE)gcc
CFLAGS = -marm -mtune=cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=hard -march=armv7ve 
LDFLAGS = -lSDL -lSDL_image -lSDL_ttf -lSDL_mixer -lmsettings

OBJS = ../common/utils.c ../common/interface.c ../common/controls.c ../common/powerops.c ../common/rumble.c ../common/settings.c

all:
	$(CC) $(TARGET).c $(OBJS) -o $(TARGET) $(CFLAGS) $(LDFLAGS)
clean:
	rm -f $(TARGET).o