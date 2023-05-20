.PHONY: all program libs build bundle release dirs clean clean-all toolchain clean-toolchain .docker
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
STATIC_DIR := $(ROOT_DIR)/static
CORES_DIR := $(ROOT_DIR)/cores
BUNDLE_LIBS =
#
ECHO:= @echo "\n::$(TARGET) V$(VERSION) build complete, enjoy!"
PATCH = git apply

LIBC_LIB_DIR := /opt/miyoomini-toolchain/arm-none-linux-gnueabihf/libc/lib
GCC_VER_GTE9_0 := $(shell echo `gcc -dumpversion | cut -f1-2 -d.` \>= 9.0 | bc )
ifeq "$(GCC_VER_GTE9_0)" "1"
  BUNDLE_LIBS=bundle
endif

TOOLCHAIN_NAME=ghcr.io/onionui/miyoomini-toolchain
COMPILE_CHAIN = libs build $(BUNDLE_LIBS) release

###########################################################

all: $(COMPILE_CHAIN)

libs:
	@echo "\n::$(TARGET) -- Compiling Libs"
# IMPORTANT: libmsettings needs to build first to compile miyoomin-toolchain dependencies 
	cd $(SRC_DIR)/libmsettings && make
	cd $(SRC_DIR)/libmmenu && make
	cd $(SRC_DIR)/batmon && make
	cd $(SRC_DIR)/keymon && make
	cd $(SRC_DIR)/lumon && make
	cd $(SRC_DIR)/app && make
	cd $(SRC_DIR)/sys-img && make
	cd $(SRC_DIR)/sys-msg && make
	cd $(SRC_DIR)/confirm && make
# Tools
	cd $(SRC_DIR)/clock && make
	cd $(SRC_DIR)/screenshots && make
	cd $(THIRD_PARTY_DIR)/DinguxCommander && make -j

dirs: clean
	@echo "\n::$(TARGET) -- Making Directories"
	@mkdir -p $(RELEASE_DIR)
	@mkdir -p $(BUILD_DIR)

build: dirs
	@echo "\n::$(TARGET) -- Copying directories"
	cp -R $(STATIC_DIR)/. $(BUILD_DIR)/dist

	mv $(BUILD_DIR)/dist/miyoo354/app/keymon.sh $(BUILD_DIR)/dist/miyoo354/app/keymon
	cp $(SRC_DIR)/libmsettings/libmsettings.so $(BUILD_DIR)/dist/.system/lib/
	cp $(SRC_DIR)/libmmenu/libmmenu.so $(BUILD_DIR)/dist/.system/lib/
	cp $(SRC_DIR)/batmon/batmon $(BUILD_DIR)/dist/.system/bin/
	cp $(SRC_DIR)/keymon/keymon $(BUILD_DIR)/dist/.system/bin/
	cp $(SRC_DIR)/lumon/lumon $(BUILD_DIR)/dist/.system/bin/
	cp $(SRC_DIR)/app/retroplay $(BUILD_DIR)/dist/.system/paks/retroplay.pak/
	cp $(SRC_DIR)/sys-img/sys-img $(BUILD_DIR)/dist/.system/bin/
	cp $(SRC_DIR)/sys-msg/sys-msg $(BUILD_DIR)/dist/.system/bin/
	cp $(SRC_DIR)/confirm/confirm $(BUILD_DIR)/dist/.system/bin/
	cp $(SRC_DIR)/sys-img/sys-img $(BUILD_DIR)/dist/miyoo354/app/
	cp $(SRC_DIR)/sys-msg/sys-msg $(BUILD_DIR)/dist/miyoo354/app/

	cp $(SRC_DIR)/clock/clock $(BUILD_DIR)/dist/Apps/Clock.pak/
	cp $(SRC_DIR)/screenshots/screenshots $(BUILD_DIR)/dist/Apps/Screenshots.pak/
	cp $(THIRD_PARTY_DIR)/DinguxCommander/output/DinguxCommander $(BUILD_DIR)/dist/Apps/Files.pak/
	cp -r $(THIRD_PARTY_DIR)/DinguxCommander/res $(BUILD_DIR)/dist/Apps/Files.pak/

# NOTE: Save build time with pre-build cores
	cp $(CORES_DIR)/picoarch $(BUILD_DIR)/dist/.system/bin/
#Arcade
	cp $(CORES_DIR)/fbalpha_libretro.so $(BUILD_DIR)/dist/.system/cores/
	cp $(CORES_DIR)/fbalpha2012_cps1_libretro.so $(BUILD_DIR)/dist/.system/cores/
	cp $(CORES_DIR)/fbalpha2012_cps2_libretro.so $(BUILD_DIR)/dist/.system/cores/
	cp $(CORES_DIR)/fbalpha2012_cps3_libretro.so $(BUILD_DIR)/dist/.system/cores/
#Nintendo
	cp $(CORES_DIR)/fceumm_libretro.so $(BUILD_DIR)/dist/.system/cores/
	cp $(CORES_DIR)/gambatte_libretro.so $(BUILD_DIR)/dist/.system/cores/
	cp $(CORES_DIR)/gpsp_libretro.so $(BUILD_DIR)/dist/.system/cores/
	cp $(CORES_DIR)/mgba_libretro.so $(BUILD_DIR)/dist/.system/cores/
	cp $(CORES_DIR)/snes9x2005_libretro.so $(BUILD_DIR)/dist/.system/cores/
	cp $(CORES_DIR)/snes9x2005_plus_libretro.so $(BUILD_DIR)/dist/.system/cores/
	cp $(CORES_DIR)/mednafen_supafaust_libretro.so $(BUILD_DIR)/dist/.system/cores/
#SEGA
	cp $(CORES_DIR)/picodrive_libretro.so $(BUILD_DIR)/dist/.system/cores/
	cp $(CORES_DIR)/genesis-plus-gx_libretro.so $(BUILD_DIR)/dist/.system/cores/
	cp $(CORES_DIR)/smsplus-gx_libretro.so $(BUILD_DIR)/dist/.system/cores/
#Playstation
	cp $(CORES_DIR)/pcsx_rearmed_libretro.so $(BUILD_DIR)/dist/.system/cores/

bundle:
# NOTE: only bundles if GCC_VER_GTE9_0 is detected? not sure if these are dependencies for device
	@echo "\n::$(TARGET) -- Bundling LIBC Libs"
	cp -L $(LIBC_LIB_DIR)/ld-linux-armhf.so.3 $(BUILD_DIR)/dist/.system/lib/
	cp -L $(LIBC_LIB_DIR)/libc/lib/libc.so.6 $(BUILD_DIR)/dist/.system/lib/
	cp -L $(LIBC_LIB_DIR)/libc/lib/libcrypt.so.1 $(BUILD_DIR)/dist/.system/lib/
	cp -L $(LIBC_LIB_DIR)/libc/lib/libdl.so.2 $(BUILD_DIR)/dist/.system/lib/
	cp -L $(LIBC_LIB_DIR)/libc/lib/libgcc_s.so.1 $(BUILD_DIR)/dist/.system/lib/
	cp -L $(LIBC_LIB_DIR)/libc/lib/libm.so.6 $(BUILD_DIR)/dist/.system/lib/
	cp -L $(LIBC_LIB_DIR)/libc/lib/libpcprofile.so $(BUILD_DIR)/dist/.system/lib/
	cp -L $(LIBC_LIB_DIR)/libc/lib/libpthread.so.0 $(BUILD_DIR)/dist/.system/lib/
	cp -L $(LIBC_LIB_DIR)/libc/lib/libresolv.so.2 $(BUILD_DIR)/dist/.system/lib/
	cp -L $(LIBC_LIB_DIR)/libc/lib/librt.so.1 $(BUILD_DIR)/dist/.system/lib/
	cp -L $(LIBC_LIB_DIR)/libc/lib/libstdc++.so.6 $(BUILD_DIR)/dist/.system/lib/

release:
	@echo "\n::$(TARGET) -- Zipping up release" 
	cd $(BUILD_DIR)/dist/.system/paks/retroplay.pak && echo "$(RELEASE_NAME).zip" > version.txt
	cd $(BUILD_DIR)/dist && zip -r retroplay.zip .system .tmp_update
	mv $(BUILD_DIR)/dist/retroplay.zip $(BUILD_DIR)/dist/miyoo354/app/
	cd $(BUILD_DIR)/dist && zip -r $(RELEASE_DIR)/$(RELEASE_NAME).zip Bios Roms Saves Apps miyoo354 README.txt
	$(ECHO)

zip-bundles: build $(BUNDLE_LIBS) zip

clean:
	@echo "\n::$(TARGET) -- Refreshing directories"
	@rm -rf $(BUILD_DIR)
	@rm -rf $(RELEASE_DIR)

clean-all: clean
	@echo "\n::$(TARGET) -- Cleaning up"
	rm -rf $(CACHE)
	rm -rf $(BUILD_DIR)
	rm -rf $(RELEASE_DIR)
	cd $(SRC_DIR)/libmsettings && make clean
	cd $(SRC_DIR)/libmmenu && make clean
	cd $(SRC_DIR)/batmon && make clean
	cd $(SRC_DIR)/keymon && make clean
	cd $(SRC_DIR)/lumon && make clean
	cd $(SRC_DIR)/app && make clean
	cd $(SRC_DIR)/sys-img && make clean
	cd $(SRC_DIR)/sys-msg && make clean
	cd $(SRC_DIR)/confirm && make clean
	cd $(THIRD_PARTY_DIR)/SDL-1.2 && make distclean
	cd $(THIRD_PARTY_DIR)/picoarch && make platform=miyoomini clean
	cd $(THIRD_PARTY_DIR)/DinguxCommander && make clean

clean-cores: clean
	@echo "\n::$(TARGET) -- Cleaning cores"
	cd $(SRC_DIR)/libmsettings && make clean
	cd $(SRC_DIR)/libmmenu && make clean
	cd $(THIRD_PARTY_DIR)/picoarch && make platform=miyoomini clean

# Third party patches, NOTE Pokemini core and MMENU flag errors out build, patched to remove them.
third-party/SDL-1.2/.patched:
	@echo "\n::$(TARGET) -- Patching SDL-1.2"
	cd $(THIRD_PARTY_DIR)/SDL-1.2 && $(PATCH) -p1 < $(ROOT_DIR)/patches/SDL-1.2/0001-vol-keys.patch && touch .patched
third-party/picoarch/.patched:
	@echo "\n::$(TARGET) -- Patching Picoarch"
# cd $(THIRD_PARTY_DIR)/picoarch && $(PATCH) -p1 < $(ROOT_DIR)/patches/picoarch/0001-picoarch.patch && touch .patched

build-libs: third-party/SDL-1.2/.patched
# NOTE: run commands to re-build dependency libs
	@echo "\n::$(TARGET) -- Compiling SDL-1.2 lib"
	cd $(SRC_DIR)/libmsettings && make
	cd $(THIRD_PARTY_DIR)/latency_reduction && make
	cd $(THIRD_PARTY_DIR)/SDL-1.2 && ./make.sh
	cp $(SRC_DIR)/libmsettings/libmsettings.so $(BUILD_DIR)/dist/.system/lib/
	cp $(THIRD_PARTY_DIR)/latency_reduction/as_preload.so $(BUILD_DIR)/dist/.system/lib/
	cp $(THIRD_PARTY_DIR)/latency_reduction/audioserver.mod $(BUILD_DIR)/dist/.system/bin/
	cp $(THIRD_PARTY_DIR)/SDL-1.2/build/.libs/libSDL-1.2.so.0.11.5 $(BUILD_DIR)/dist/.system/lib/libSDL-1.2.so.0

build-cores: third-party/picoarch/.patched
# NOTE: run commands to re-build cores
# Fbalpha2012 cores provided else where
	@echo "\n::$(TARGET) -- Pulling and compiling Picoarch cores for Miyoo Mini"
	cd $(SRC_DIR)/libmsettings && make
	cd $(SRC_DIR)/libmmenu && make
	cd $(THIRD_PARTY_DIR)/picoarch && make platform=miyoomini -j
	cp $(THIRD_PARTY_DIR)/picoarch/output/picoarch $(CORES_DIR)
	cp $(THIRD_PARTY_DIR)/picoarch/output/fceumm_libretro.so $(CORES_DIR)
	cp $(THIRD_PARTY_DIR)/picoarch/output/gambatte_libretro.so $(CORES_DIR)
	cp $(THIRD_PARTY_DIR)/picoarch/output/gpsp_libretro.so $(CORES_DIR)
	cp $(THIRD_PARTY_DIR)/picoarch/output/pcsx_rearmed_libretro.so $(CORES_DIR)
	cp $(THIRD_PARTY_DIR)/picoarch/output/picodrive_libretro.so $(CORES_DIR)
	cp $(THIRD_PARTY_DIR)/picoarch/output/snes9x2005_plus_libretro.so $(CORES_DIR)
	cp $(THIRD_PARTY_DIR)/picoarch/output/mednafen_supafaust_libretro.so $(CORES_DIR)
	cp $(THIRD_PARTY_DIR)/picoarch/output/mgba_libretro.so $(CORES_DIR)

# Init git submodules
git-submodules:
	@echo "\n::$(TARGET) -- Pulling git submodule dependencies"
	git submodule update --init --recursive

# Docker toolchain setup
.docker: Dockerfile
	@chmod a+x $(DOCKER_DIR)/support/setup-toolchain.sh
	@chmod a+x $(DOCKER_DIR)/support/setup-env.sh
	@chmod a+x $(DOCKER_DIR)/support/setup-sqlite.sh
	@docker build -t $(TOOLCHAIN_NAME) .
	@touch $(DOCKER_DIR)/.docker

# Build toolchain
toolchain: .docker
	@echo "\n::$(TARGET) -- Miyoo Mini toolchain is ready"
	@docker run -it --rm -v "$(ROOT_DIR)":/root/workspace $(TOOLCHAIN_NAME) /bin/bash

clean-toolchain:
	@echo "\n::$(TARGET) -- Clearing toolchain cache and image"
	docker rmi $(TOOLCHAIN_NAME)
	rm -f $(DOCKER_DIR)/.docker