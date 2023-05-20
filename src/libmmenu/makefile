.PHONY: all clean

TARGET = mmenu

ifeq (,$(PLATFORM))
PLATFORM=$(UNION_PLATFORM)
endif
ifeq (,$(PLATFORM))
$(error please specify PLATFORM, eg. make PLATFORM=trimui)
endif
ifeq (,$(CROSS_COMPILE))
$(error missing CROSS_COMPILE for this toolchain)
endif
ifeq (,$(PREFIX))
$(error missing PREFIX for installation in toolchain)
endif


CC = $(CROSS_COMPILE)gcc

SYSROOT := $(shell $(CC) --print-sysroot)

INCLUDEDIR = $(SYSROOT)/usr/include

CFLAGS = -I$(INCLUDEDIR) -I. -I../common -DPLATFORM_$(shell echo $(PLATFORM) | tr a-z A-Z) -std=gnu99
LDFLAGS = -s -lSDL -lSDL_image -lSDL_ttf -lSDL_mixer -lmsettings -lz -lm -Wl,--no-as-needed -ldl -lrt
CFLAGS += -ggdb3 -Og

ifeq (trimui,$(PLATFORM))
LDFLAGS += -ltinyalsa
endif
ifeq (rs90,$(PLATFORM))
LDFLAGS += -lasound
endif

OPTM=-Ofast

OBJS = ../common/utils.c ../common/interface.c ../common/controls.c

all:
	$(CC) -c -Werror -fpic $(TARGET).c $(OBJS) $(CFLAGS) $(LDFLAGS) $(OPTM)
	$(CC) -shared -o lib$(TARGET).so $(TARGET).o utils.o interface.o controls.o
	cp $(TARGET).h $(PREFIX)/include
	cp lib$(TARGET).so $(PREFIX)/lib

clean:
	rm -f *.o
	rm -f lib$(TARGET).so
	rm -f $(PREFIX)/include/$(TARGET).h
	rm -f $(PREFIX)/lib/lib$(TARGET).so