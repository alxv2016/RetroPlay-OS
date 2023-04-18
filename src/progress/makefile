.PHONY: all clean
	
###########################################################

ifeq (,$(CROSS_COMPILE))
$(error missing CROSS_COMPILE for this toolchain)
endif

TARGET = progress

###########################################################

CC = $(CROSS_COMPILE)gcc
SYS_ROOT := $(shell $(CC) --print-sysroot)
INCLUDE_DIR = $(SYS_ROOT)/usr/include
CFLAGS = -I$(INCLUDE_DIR) -I. -I../common -DPLATFORM_$(shell echo $(PLATFORM) | tr a-z A-Z) -std=gnu99
LDFLAGS = -s -lSDL -lSDL_image -lSDL_ttf -lmsettings -lpthread -lrt -lz -lm
OPTM=-O3

OBJS = ../common/utils.c ../common/interface.c

all:
	$(CC) $(TARGET).c $(OBJS) -o $(TARGET) $(CFLAGS) $(LDFLAGS) $(OPTM) -ldl -rdynamic

clean:
	rm -f $(TARGET).o