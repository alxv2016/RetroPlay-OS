.PHONY: all clean

ifeq (,$(CROSS_COMPILE))
$(error missing CROSS_COMPILE for this toolchain)
endif

TARGET = screenshot

CC = $(CROSS_COMPILE)gcc
CFLAGS   = -Os -marm -mtune=cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=hard -march=armv7ve+simd 
CFLAGS += -ffunction-sections -fdata-sections -Wall
LDFLAGS	 = -lpng -Wl,--gc-sections -s

OBJS = ../common/utils.c ../common/interface.c ../common/controls.c ../common/rumble.c 

all:
	$(CC) $(TARGET).c -o $(TARGET) $(CFLAGS) $(LDFLAGS)
clean:
	rm -f $(TARGET).o
