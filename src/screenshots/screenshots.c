#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>
#include <msettings.h>

#include "../common/defines.h"
#include "../common/keycontext.h"

#include "../common/utils.h"
#include "../common/controls.h"
#include "../common/interface.h"
#include "../common/rumble.h"
#include "../common/gallery.h"

#include "screenshots.h"
///////////////////////////////////
static SDL_Surface *overlay;

int main(int argc, char *argv[]) {
  rumble(OFF);
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
  SDL_ShowCursor(0);
  SDL_EnableKeyRepeat(500, 50);
  InitSettings();
  gfx.screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 16, SDL_HWSURFACE | SDL_DOUBLEBUF);
  if (Mix_OpenAudio(48000, 32784, 2, 4096) < 0) return 0;
  GFX_init();
  GFX_ready();
  Input_reset();

  int dirty = 1;
  int quit = 0;
  int selected = 0;
  int totalScreenshots = getScreenshotTotal(SCREENSHOT_PATH);
  char **screenshotPaths;

  while (!quit) {
    unsigned long frameStart = SDL_GetTicks();
    Input_poll();

  if (totalScreenshots > 0) {
    if (Input_justPressed(BTN_UP)) {
      clearScreenshot();
      playArrowSound();
      selected -= 1;
      if (selected < 0) {
        selected += totalScreenshots;
      }
      dirty = 1;
    } else if (Input_justPressed(BTN_DOWN)) {
      clearScreenshot();
      playArrowSound();
      selected += 1;
      if (selected >= totalScreenshots) {
        selected -= totalScreenshots;
      }
      dirty = 1;
    }
  }

  if (Input_justPressed(BTN_B)) {
    playClickSound();
    SDL_Delay(200);
    quit = 1;
  }

    if (dirty) {
      SDL_FillRect(gfx.screen, NULL, 0);
      if (totalScreenshots > 0) {
        if (loadScreenshots(SCREENSHOT_PATH, &screenshotPaths, &totalScreenshots)) {
          showScreenshot(selected, screenshotPaths, gfx.screen);
          } else {
            SDL_FreeSurface(gfx.screen);
            SDL_Quit();
            return 0;
        }
        SDL_Surface *countInfo;
        char tmp[256];
        sprintf(tmp, "%i of %i Screenshots", selected + 1, totalScreenshots);
        countInfo = TTF_RenderUTF8_Blended(font.body, tmp, (SDL_Color){WHITE});

        overlay = SDL_CreateRGBSurface(SDL_SWSURFACE, SCREEN_WIDTH, BUTTON_SIZE + SPACING_XXL, 16, 0, 0, 0, 0);
        SDL_SetAlpha(overlay, SDL_SRCALPHA, 0x98);
        SDL_FillRect(overlay, NULL, 0);
        SDL_BlitSurface(overlay, NULL, gfx.screen, &(SDL_Rect){0, SCREEN_HEIGHT - overlay->h});

        int cy = (BUTTON_SIZE / 2) - (BODY / 2);
        SDL_BlitSurface(countInfo, NULL, gfx.screen, &(SDL_Rect){SPACING_XL, SCREEN_HEIGHT - SPACING_LG + cy - BUTTON_SIZE});
        SDL_FreeSurface(countInfo);
        tertiaryBTN(gfx.screen, "View", 2, 1, SCREEN_WIDTH - SPACING_LG - 114, SCREEN_HEIGHT - SPACING_LG);
      } else {
        emptyState(gfx.screen, gfx.empty_screenshots, H2, CAPTION, "L2 + R2 to take screenshots", "There are no screenshots to view.");
      }
      primaryBTN(gfx.screen, "B", "Close", 1, SCREEN_WIDTH - SPACING_LG, SCREEN_HEIGHT - SPACING_LG);

      SDL_Flip(gfx.screen);
      dirty = 0;
    }
    // slow down to 60fps
    GFX_sync(frameStart);
  }

  if (totalScreenshots > 0 && screenshotPaths) {
    for (int i = 0; i < totalScreenshots; i++) {
      free(screenshotPaths[i]);
    }
    free(screenshotPaths);
    clearScreenshot();
  }

  if (overlay) SDL_FreeSurface(overlay);

  GFX_quit();
  SDL_Quit();
  freeSound();
  QuitSettings();
  return 0;
}

/* Previous, Current, Next logic incase we want to re-address this implementation
For now it doesn't seem to make a difference and causing issues */
// if (Input_justPressed(BTN_UP)) {
//       cleanImagesCache();
//       playArrowSound();
//       // clearImageCache(0, &cache_used);
//       selected -= 1;
//       next = selected;
//       prev = selected;
//       ++next;
//       --prev;
//       if (selected < 0) {
//         selected = total - 1;
//         prev = selected - 1;
//         next = 0;
//       }
//       if (prev < 0) {
//         prev = total - 1;
//       }
//       dirty = 1;
//     } else if (Input_justPressed(BTN_DOWN)) {
//       ///clearImageCache(1, &cache_used);
//       cleanImagesCache();
//       playArrowSound();
//       selected += 1;
//       next = selected;
//       prev = selected;
//       ++next; 
//       --prev;
//       if (next >= total) {
//         next = 0;
//       }
//       if (selected >= total) {
//         selected = 0;
//         prev = total - 1;
//       }
//       dirty = 1;
//     } else if (Input_justPressed(BTN_B)) {
//       playClickSound();
//       SDL_Delay(200);
//       quit = 1;
//     }