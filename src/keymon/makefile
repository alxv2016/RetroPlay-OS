.PHONY: all clean

ifeq (,$(CROSS_COMPILE))
$(error missing CROSS_COMPILE for this toolchain)
endif

TARGET = keymon

CC = $(CROSS_COMPILE)gcc
FLAGS	= -Os -lmsettings -lpthread -lrt -ldl -Wl,--gc-sections -s

OBJS = ../common/rumble.c

all:
	$(CC) $(TARGET).c $(OBJS) -o $(TARGET) $(FLAGS)

clean:
	rm -rf $(TARGET).o