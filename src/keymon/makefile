.PHONY: all clean

ifeq (,$(CROSS_COMPILE))
$(error missing CROSS_COMPILE for this toolchain)
endif

TARGET = keymon

CC = $(CROSS_COMPILE)gcc
FLAGS	= -Os -lmsettings -lpthread -lrt -ldl -Wl,--gc-sections -s

all:
	$(CC) -o keymon $(TARGET).c ../common/rumble.c $(FLAGS)
clean:
	rm -rf keymon