.PHONY: all clean

ifeq (,$(CROSS_COMPILE))
$(error missing CROSS_COMPILE for this toolchain)
endif

TARGET = keymon

CC = $(CROSS_COMPILE)gcc
FLAGS	= -Os -lmsettings -lSDL -lSDL_mixer -lpthread -lrt -ldl -Wl,--gc-sections -s

OBJS = ../common/rumble.c

all:
	$(CC) $(TARGET).c -o $(TARGET) $(FLAGS)

clean:
	rm -rf $(TARGET).o