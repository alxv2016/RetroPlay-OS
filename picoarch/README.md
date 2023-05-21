# picoarch - a libretro frontend designed for small screens and low power

picoarch uses libpicofe and SDL to create a small frontend to libretro cores. It's designed for small (320x240 2.0-2.4") screen, low-powered devices like the Trimui Model S (PowKiddy A66).

## Running

picoarch can be run by specifying the core library and the content to run:

```
./picoarch /path/to/core_name_libretro.so /path/to/game.gba
```

If you do not specify core or content, picoarch will have you select a core from the current directory and content using the built-in file browser.

## Building

The frontend can currently be built for the TrimUI Model S and Linux (useful for testing and debugging).

First, fetch the repo with submodules:

```
git clone --recurse-submodules https://git.crowdedwood.com/picoarch
```

### Linux instructions

To build picoarch itself, you need libSDL 1.2, libpng, and libasound. Different cores may need additional dependencies.

After that, `make` builds picoarch and all supported cores into this directory.

### TrimUI instructions

To build for TrimUI, you need to set up the [toolchain](https://git.crowdedwood.com/trimui-toolchain/about/) first.

To build generic binaries:

```
make platform=trimui
```

If you want to build for MinUI, you need to install [libmmenu](https://github.com/shauninman/libmmenu) into the toolchain. Then:

```
make platform=trimui MINUI=1
```

`MINUI=1` will change save/config/system paths to match MinUI standards. If you just want to include mmenu, you can run:

```
make platform=trimui MMENU=1
```

### Other build options

To debug:

```
make DEBUG=1
```

To build a specific supported core:

```
make gpsp_libretro.so
```

To clean a core so it will be built again:

```
make clean-gpsp
```

To completely clean the repo (will delete, pull, and patch all core repos from scratch)

```
make force-clean
```

Distribution builds can also be made for gmenunx and MinUI:

```
make platform=trimui dist-gmenu
make platform=trimui MINUI=1 dist-minui
```

These will output a directory structure that can be moved onto the SD card into `pkg/gmenunx` or `pkg/MinUI`.

To build profiles for profile-guided optimization:

```
make PROFILE=GENERATE
```

To apply the generated profiles:

```
make PROFILE=APPLY
```

PGO can give noticeable speed improvements with some emulators.

## Notes on cores

In order to make development and testing easier, the Makefile will pull and build supported cores.

You will have to make changes when adding a core, since TrimUI is not a supported libretro platform. picoarch has a `patches/` directory containing needed changes to make cores work well in picoarch. Patches are applied in order after checking out the repository. 

At a minimum, you need to add a `platform=trimui` section to the core Makefile.

Some features and fixes are also included in `patches` -- it would be best to try to upstream them.

picoarch keeps the running core name in a global variable. This is used to override defaults and core settings to work more nicely within picoarch. Overrides based on core name are kept in `overrides/` and referenced in `overrides.c`. These are used to:

- Shorten core option text and change defaults for small screen / low power devices
- Rename buttons to match the core's system
- Reference frameskip core options to make fast-forward faster
- Display extra options or hide unnecessary options

# picoarch - a libretro frontend designed for small screens and low power

picoarch runs libretro cores (emulators) for various systems with low overhead and UI designed for small screen, low-powered devices like the Trimui Model S (PowKiddy A66).

It supports: 

- **Arcade** (mame2000)
- **Colecovision** (blueMSX, smsplus)
- **Game Boy / Game Boy Color** (gambatte)
- **Game Boy Advance** (gpsp)
- **Game Gear** (picodrive, smsplus)
- **Genesis** (picodrive)
- **MSX** (fMSX, blueMSX)
- **NES** (quicknes, fceumm)
- **Sega Master System** (picodrive, smsplus)
- **Super NES** (snes9x2002, snes9x2005)
- **PCE / TurboGrafx-16** (beetle-pce-fast)
- **PlayStation** (pcsx_rearmed)
- more to come

picoarch can also play game music (gme).

All emulators have:

- fast-forward
- soft scaling options
- menu+button combo keybindings
- per-game config
- screenshots

Most have:
- cheat support
- IPS/BPS softpatching
- auto-frameskip for smooth audio

## Install

### gmenunx

Extract the contents of the `gmenunx/` directory onto the SD card. You will have a `libretro` section with launchers for all of the emulators. You will also have `picoarch` in the emulators section where you can run any core in `/Apps/picoarch/`.

### MinUI

If you want to replace built-in .paks, simply extract `MinUI/` onto the SD card. 

If you want to replace individual emulators, overwrite the existing MinUI .pak with the picoarch version.

If you want to use picoarch as a single application instead of replacing .paks, copy `MinUI/Games` onto the SD card.

## Notes / extra features

### Bios

Some emulators require bios files. For gmenunx, bios files are placed into `/Apps/picoarch/system`. For MinUI, they are placed in the save directory, similar to other .paks.

The libretro documentation specifies which bios is required for each core. For example, needed fMSX bios files are listed here: <https://docs.libretro.com/library/fmsx/>

### Cheats

Cheats use RetroArch .cht file format. Many cheat files are here <https://github.com/libretro/libretro-database/tree/master/cht>

Cheat file name needs to match ROM name, and go underneath save directory. For example, `/Apps/.picoarch-gambatte/cheats/Super Mario Land (World).cht`. When a cheat file is detected, a "cheats" menu item will appear. Not all cheats work with all cores, may want to clean up files to just the cheats you want.

### IPS / BPS soft-patching

Many cores can apply patches when loading. For example, loading `/roms/game.gba` will apply patches named `/roms/game.ips`, `/roms/game.ips1`, `/roms/game.IPS2`, `/roms/game.bps`, etc. Patching is temporary, original files are unmodified. Patches are loaded in case-insensitive alphabetical order. Note that `.ips12` loads before `.ips2`, but after `.ips02`.
