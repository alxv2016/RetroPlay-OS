#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <msettings.h>
#include <dlfcn.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../common/common.h"
#include "mmenu.h"

MenuReturnStatus ShowMenu(char* rom_path, char* save_path_template, SDL_Surface* optional_snapshot, MenuRequestState requested_state, AutoSave_t autosave) {
  return EXIT_SUCCESS;
}

void ShowWarning(void) {
  int demo =1;
}

int ResumeSlot(void) {
	return 0;
}

int ChangeDisc(char* disc_path) {
	return 1;
}