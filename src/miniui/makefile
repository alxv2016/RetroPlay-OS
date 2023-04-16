.PHONY: build
.PHONY: clean
	
###########################################################

ifeq (,$(PLATFORM))
PLATFORM=$(UNION_PLATFORM)
endif

ifeq (,$(PLATFORM))
$(error please specify PLATFORM, eg. make PLATFORM=trimui)
endif
ifeq (,$(CROSS_COMPILE))
$(error missing CROSS_COMPILE for this toolchain)
endif

TARGET = main

###########################################################

CC = $(CROSS_COMPILE)gcc
SYSROOT := $(shell $(CC) --print-sysroot)
INCLUDEDIR = $(SYSROOT)/usr/include
CFLAGS = -I$(INCLUDEDIR) -I. -I../common -DPLATFORM_$(shell echo $(PLATFORM) | tr a-z A-Z) -std=gnu99
LDFLAGS = -s -lSDL -lSDL_image -lSDL_ttf -lmsettings -lrt -lz -lm
OPTM=-O3

OBJS = ../common/interface.c ../common/controls.c ../common/powerops.c ../common/utils.c ../common/api.c

build:
	$(CC) -o MiniUI $(TARGET).c $(OBJS) $(CFLAGS) $(LDFLAGS) $(OPTM) -ldl -rdynamic
clean:
	rm -f MiniUI