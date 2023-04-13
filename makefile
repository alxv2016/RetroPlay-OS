# .PHONY: shell
.PHONY: patch all dist build clean toolchain clean-toolchain .docker

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

all: dist

dist: build
	$(ECHO)
	cd $(SRC_DIR)/libmsettings && make

build:
	$(ECHO)
	cd $(THIRD_PARTY_DIR)/picoarch && make platform=miyoomini -j

clean:
	cd $(THIRD_PARTY_DIR)/picoarch && make clean

patch:
	cd $(THIRD_PARTY_DIR)/picoarch && $(PATCH) -p1 < ../../patches/picoarch/0001-picoarch.patch && touch .patched

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