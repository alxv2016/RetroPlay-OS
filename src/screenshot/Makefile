.PHONY: all clean

ifeq (,$(CROSS_COMPILE))
$(error missing CROSS_COMPILE for this toolchain)
endif

TARGET = screenshot

CC = $(CROSS_COMPILE)gcc
CFLAGS = -marm -mtune=cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=hard -march=armv7ve 
# CFLAGS = -Os -marm -mtune=cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=hard -march=armv7ve+simd 
CFLAGS += -ffunction-sections -fdata-sections
LDFLAGS = -lSDL -lSDL_image -lSDL_ttf -lSDL_mixer -lmsettings -lpng -Wl,--gc-sections -s

OBJS = ../common/utils.c ../common/interface.c ../common/controls.c ../common/rumble.c ../common/gallery.c

all:
	$(CC) $(TARGET).c $(OBJS) -o $(TARGET) $(CFLAGS) $(LDFLAGS)
clean:
	rm -f $(TARGET).o
