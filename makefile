# .PHONY: shell
.PHONY: patch all lib build external release clean toolchain clean-toolchain .docker

ifeq (,$(PLATFORM))
PLATFORM=$(UNION_PLATFORM)
endif

###########################################################

TARGET=RetroPlayOS
VERSION=1.0-alpha
RELEASE_NAME=$(TARGET)-v$(VERSION)

###########################################################

#Directories
ROOT_DIR := $(shell pwd)
DOCKER_DIR := $(ROOT_DIR)/docker
SRC_DIR := $(ROOT_DIR)/src
THIRD_PARTY_DIR := $(ROOT_DIR)/third-party
BUILD_DIR := $(ROOT_DIR)/build
RELEASE_DIR := $(ROOT_DIR)/release
DIST_DIR := $(ROOT_DIR)/dist
EXTRAS_DIR := $(ROOT_DIR)/extras
# 
ECHO:= echo "\n::Building Demo!"

PATCH = git apply

TOOLCHAIN_NAME=ghcr.io/onionui/miyoomini-toolchain

LIBC_LIB=/opt/miyoomini-toolchain/arm-none-linux-gnueabihf/libc/lib
BUNDLE_LIBS=

GCC_VER_GTE9_0 := $(shell echo `gcc -dumpversion | cut -f1-2 -d.` \>= 9.0 | bc )
ifeq "$(GCC_VER_GTE9_0)" "1"
  BUNDLE_LIBS=bundle
endif

all: lib

lib: external
	$(ECHO)
	cd $(SRC_DIR)/libmsettings && make
	cd $(THIRD_PARTY_DIR)/SDL-1.2 && ./make.sh
	cd $(THIRD_PARTY_DIR)/latency_reduction && make

external: build
	$(ECHO)
	cd $(THIRD_PARTY_DIR)/picoarch && make platform=miyoomini -j

build: release
	$(ECHO)
	mkdir -p $(RELEASE_DIR)
	mkdir -p $(BUILD_DIR)
	cp -R $(DIST_DIR)/. $(BUILD_DIR)/dist

release:
	$(ECHO)
	cd $(BUILD_DIR)/dist/.system/paks/MiniUI.pak && echo "$(RELEASE_NAME).zip" > version.txt
	cd $(BUILD_DIR)/dist && zip -r MiniUI.zip .system .tmp_update
	mv $(BUILD_DIR)/dist/MiniUI.zip $(BUILD_DIR)/dist/miyoo354/app/
	cd $(BUILD_DIR)/dist && zip -r $(RELEASE_DIR)/$(RELEASE_NAME).zip Bios Roms Saves miyoo354 README.txt

clean:
	cd $(THIRD_PARTY_DIR)/picoarch && make clean

# Run patch manually before build
patch:
	cd $(THIRD_PARTY_DIR)/picoarch && $(PATCH) -p1 < ../../patches/picoarch/0001-picoarch.patch && touch .patched
	cd $(THIRD_PARTY_DIR)/SDL-1.2 && $(PATCH) -p1 < ../../patches/SDL-1.2/0001-vol-keys.patch && touch .patched

# Docker toolchain setup
.docker: Dockerfile
	chmod a+x $(DOCKER_DIR)/support/setup-toolchain.sh
	chmod a+x $(DOCKER_DIR)/support/setup-env.sh
	chmod a+x $(DOCKER_DIR)/support/setup-sqlite.sh
	docker build -t $(TOOLCHAIN_NAME) .
	touch $(DOCKER_DIR)/.docker

toolchain: .docker
	docker run -it --rm -v "$(ROOT_DIR)":/root/workspace $(TOOLCHAIN_NAME) /bin/bash

clean-toolchain:
	docker rmi $(TOOLCHAIN_NAME)
	rm -f $(DOCKER_DIR)/.docker