.PHONY: all clean

ifeq (,$(CROSS_COMPILE))
$(error missing CROSS_COMPILE for this toolchain)
endif
ifeq (,$(PREFIX))
$(error missing PREFIX for this toolchain)
endif

TARGET = msettings

CC = $(CROSS_COMPILE)gcc
SYS_ROOT := $(shell $(CC) --print-sysroot)
INCLUDE_DIR = $(SYS_ROOT)/usr/include
CFLAGS = -I$(INCLUDE_DIR)
LDFLAGS = -lcam_os_wrapper -lmi_sys -lmi_ao -ldl -lrt -s -lm

OPTM=-Ofast

all: 
	$(CC) -c -Werror -fpic "$(TARGET).c" -Wl,--no-as-needed $(LDFLAGS)
	$(CC) -shared -o "lib$(TARGET).so" "$(TARGET).o" $(LDFLAGS)
	cp "$(TARGET).h" "$(PREFIX)/include"
	cp "lib$(TARGET).so" "$(PREFIX)/lib"
	
clean:
	rm -f *.o
	rm -f "lib$(TARGET).so"
	rm -f $(PREFIX)/include/$(TARGET).h
	rm -f $(PREFIX)/lib/lib$(TARGET).so