.PHONY: all clean

ifeq (,$(CROSS_COMPILE))
$(error missing CROSS_COMPILE for this toolchain)
endif

TARGET = keymon

CC = $(CROSS_COMPILE)gcc
LDFLAGS	= -Os -lmsettings -lSDL -lSDL_mixer -lpng -lpthread -lrt -ldl -Wl,--gc-sections -s

OBJS = ../common/screencapture.c ../common/rumble.c

all:
	$(CC) $(TARGET).c $(OBJS) -o $(TARGET) $(LDFLAGS)

clean:
	rm -rf $(TARGET).o