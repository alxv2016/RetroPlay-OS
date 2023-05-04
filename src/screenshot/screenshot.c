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

#include "screenshot.h"
///////////////////////////////////

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
  int prev = 0;
  int next = 0;
  int imageCount = 0;
  int cache_used = 0;
  char **g_images_paths;

  while (!quit) {
    unsigned long frameStart = SDL_GetTicks();
    imageCount = getImagesCount(SCREENSHOT_PATH);
    int total = imageCount;
    if (selected == 0) {
      prev = total -1;
      next = selected + 1;
    } else {
      prev = selected;
      next = selected;
    }

    Input_poll();

    if (Input_justRepeated(BTN_UP)) {
      playArrowSound();
      selected -= 1;
      next = selected;
      prev = selected;
      ++next;
      --prev;
      if (selected < 0) {
        selected = total - 1;
        prev = selected - 1;
        next = 0;
      }
      if (prev < 0) {
        prev = total - 1;
      }
      dirty = 1;
    } else if (Input_justRepeated(BTN_DOWN)) {
      playArrowSound();
      selected += 1;
      next = selected;
      prev = selected;
      ++next; 
      --prev;
      if (next >= total) {
        next = 0;
      }
      if (selected >= total) {
        selected = 0;
        next = 1;
        prev = total - 1;
      }
      dirty = 1;
    } else if (Input_justPressed(BTN_B)) {
      playClickSound();
      SDL_Delay(200);
      quit = 1;
    }

    if (dirty) {
      SDL_FillRect(gfx.screen, NULL, 0);
      if (total > 0) {

      if (loadImagesPathsFromDir(SCREENSHOT_PATH, &g_images_paths, &total)) {
          // drawImageByIndex(selected, current, g_images_paths, total, gfx.screen, NULL, &cache_used);
          putInt(DEBUG_PATH, total);
        } else {
          SDL_FreeSurface(gfx.screen);
          SDL_Quit();
          return 0;
      }
        char tmp[256];
        sprintf(tmp, "prev: %i current: %i next: %i",prev, selected, next);
        paragraph(BODY, 1, tmp, (SDL_Color){WHITE}, gfx.screen, NULL);
      }

      primaryBTN(gfx.screen, "A", "Okay", 1, SCREEN_WIDTH - SPACING_LG, SCREEN_HEIGHT - SPACING_LG);
      secondaryBTN(gfx.screen, "B", "Close", 1, SCREEN_WIDTH - SPACING_LG - 113, SCREEN_HEIGHT - SPACING_LG);

      SDL_Flip(gfx.screen);
      dirty = 0;
    }
    // slow down to 60fps
    GFX_sync(frameStart);
  }

  if (g_images_paths != NULL) {
    for (int i = 0; i < imageCount; i++)
      free(g_images_paths[i]);
    free(g_images_paths);
  }

  cleanImagesCache();
  GFX_quit();
  SDL_Quit();
  freeSound();
  QuitSettings();
  return 0;
}


// static char **g_images_paths;
// static char **g_images_titles;
// static int g_images_paths_count = 0;
// static int g_image_index = -1;


// int main(int argc, char *argv[]) {
//   rumble(OFF);
//   SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
//   SDL_ShowCursor(0);
//   SDL_EnableKeyRepeat(500, 50);
//   gfx.screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 16,
//                                 SDL_HWSURFACE | SDL_DOUBLEBUF);
//   if (Mix_OpenAudio(48000, 32784, 2, 4096) < 0) return 0;
//   GFX_init();
//   GFX_ready();
//   Input_reset();

//   int dirty = 1;
//   int quit = 0;
//   int selected = 0;
//   // int cache_used = 0;
//   // char image_path[STR_MAX] = "";
//   // char images_dir_path[STR_MAX] = "";

//   // for (int i = 1; i < argc; i++) {
//   //   if (argv[i][0] == '-') {
//   //     if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--image") == 0)
//   //       strncpy(image_path, argv[++i], STR_MAX - 1);
//   //     else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--directory") == 0)
//   //       strncpy(images_dir_path, argv[++i], STR_MAX - 1);
//   //   }
//   // }

//   while (!quit) {
//     unsigned long frameStart = SDL_GetTicks();
//     int total = 7;
//     Input_poll();

//     if (Input_justPressed(BTN_UP)) {
//       playArrowSound();
//       selected -= 1;
//       if (selected < 0) {
//         selected += 4;
//       }
//       dirty = 1;
//     } else if (Input_justPressed(BTN_DOWN)) {
//       playArrowSound();
//       selected += 1;
//       if (selected >= 4) {
//         selected -= 4;
//       }
//       dirty = 1;
//     }
    
//     // else if (Input_justPressed(BTN_A)) {
//     //   playClickSound();
//     //   SDL_Delay(200);
//     //   superShortPulse();
//     //   dirty = 1;
//     // } else if (Input_justPressed(BTN_B)) {
//     //   playClickSound();
//     //   SDL_Delay(200);
//     //   quit = 1;
//     // }

//     if (dirty) {

//       // if (exists(image_path)) {
//       //   g_images_paths_count = 1;
//       //   g_image_index = 0;
//       //   drawImage(image_path, gfx.screen);
//       // } else if (exists(images_dir_path)) {
//       //   if (loadImagesPathsFromDir(images_dir_path, &g_images_paths, &g_images_paths_count) && g_images_paths_count > 0) {
//       //     g_image_index = 0;
//       //     drawImageByIndex(0, g_image_index, g_images_paths, g_images_paths_count, gfx.screen, NULL, &cache_used);
//       //     putInt(DEBUG_PATH, g_images_paths_count);
//       //   } else {
//       //     SDL_FreeSurface(gfx.screen);
//       //     SDL_Quit();
//       //     return EXIT_FAILURE;
//       //   }
//       // }
//       if (total > 0) {
//         char tmp[256];
//         sprintf(tmp, "%i", selected);
//         paragraph(BODY, 1, tmp, (SDL_Color){WHITE}, gfx.screen, NULL);
//       }
//       primaryBTN(gfx.screen, "A", "Okay", 1, SCREEN_WIDTH - SPACING_LG,
//                  SCREEN_HEIGHT - SPACING_LG);
//       secondaryBTN(gfx.screen, "B", "Close", 1, SCREEN_WIDTH - SPACING_LG - 113,
//                    SCREEN_HEIGHT - SPACING_LG);

//       SDL_Flip(gfx.screen);
//       dirty = 0;
//     }
//     // slow down to 60fps
//     GFX_sync(frameStart);
//   }

//   // if (g_images_paths != NULL) {
//   //   for (int i = 0; i < g_images_paths_count; i++)
//   //     free(g_images_paths[i]);
//   //   free(g_images_paths);
//   // }
//   // if (g_images_titles != NULL) {
//   //   for (int i = 0; i < g_images_paths_count; i++)
//   //     free(g_images_titles[i]);
//   //   free(g_images_titles);
//   // }

//   // cleanImagesCache();
//   GFX_quit();
//   SDL_Quit();
//   freeSound();

//   return 0;
// }
