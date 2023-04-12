# .PHONY: shell
.PHONY: clean

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

PATCH = git apply

TOOLCHAIN_NAME=ghcr.io/onionui/miyoomini-toolchain

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