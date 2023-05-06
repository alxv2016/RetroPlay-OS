#include <ctype.h>
#include <dirent.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "../common/defines.h"
#include "../common/utils.h"
#include "gallery.h"

static SDL_Surface *currentScreenshot = NULL;

static int getScreenshotDir(char *screenshotdir, struct dirent *ent, char *imagePath) {
  int extSize = 50;
  char ext[50];
  char *filename = ent->d_name;
  if (filename[0] == '.' || S_ISDIR(ent->d_type & DT_DIR)) {
    return 0;
  }
  strncpy(ext, getFilenameExt(filename), extSize);
  char *fileExt = toLower(ext);
  if (strcmp(fileExt, "png") == 0 || strcmp(fileExt, "jpg") == 0 ||
      strcmp(fileExt, "jpeg") == 0) {
    char fullPath[MAX_PATH];
    sprintf(fullPath, "%s%s", screenshotdir, filename);
    strcpy(imagePath, fullPath);
    return 1;
  }
  return 0;
}

static int compareStrings(const void *a, const void *b) {
  const char *aa = *(const char **)a;
  const char *bb = *(const char **)b;
  return strcmp(aa, bb);
}

int getScreenshotTotal(char *screenshotdir) {
  int imagesCount = 0;
  DIR *dir = opendir(screenshotdir);
  if (dir == NULL) {
    return 0;
  }
  struct dirent *ent;
  while ((ent = readdir(dir)) != NULL) {
    char imagePath[MAX_PATH];
    int isImage = getScreenshotDir(screenshotdir, ent, imagePath);
    if (isImage) {
      imagesCount++;
    }
  }
  closedir(dir);
  return imagesCount;
}

int loadScreenshots(char *screenshotDir, char ***imagePaths, int *totalPaths) {
  char normalizedDirPath[MAX_PATH];
  int dirPathLength = strlen(screenshotDir);
  if (dirPathLength == 0) {
    return 0;
  }
  if (screenshotDir[dirPathLength - 1] != '/') {
    sprintf(normalizedDirPath, "%s/", screenshotDir);
  } else {
    strcpy(normalizedDirPath, screenshotDir);
  }

  int totalImages = getScreenshotTotal(normalizedDirPath);
  DIR *dir = opendir(normalizedDirPath);

  if (dir == NULL) {
    return 0;
  }

  struct dirent *ent;

  *totalPaths = 0;
  *imagePaths = (char **)malloc(totalImages * sizeof(char *));

  while ((ent = readdir(dir)) != NULL) {
    char imagePath[MAX_PATH];
    int isImage = getScreenshotDir(normalizedDirPath, ent, imagePath);

    if (!isImage) {
      continue;
    }

    (*imagePaths)[*totalPaths] = (char *)malloc(MAX_PATH * sizeof(char));
    strcpy((*imagePaths)[*totalPaths], imagePath);
    (*totalPaths)++;

    if ((*totalPaths) >= totalImages) {
      // we found more images than allocated memory
      // TODO: handle this
      break;
    }
  }
  closedir(dir);
  qsort(*imagePaths, *totalPaths, sizeof(char *), compareStrings);

  return 1;
}

static void renderImage(SDL_Surface *image, SDL_Surface *screen) {
  if (!image) return;
  int16_t x = 320 - (int16_t)(image->w / 2);
  int16_t y = (int16_t)(240 - image->h / 2);
  SDL_Rect imageRect = {x, y};
  SDL_BlitSurface(image, NULL, screen, &imageRect);
}

void showScreenshot(int selected, char **screenshots, SDL_Surface *screen) {
  currentScreenshot = IMG_Load(screenshots[selected]);
  renderImage(currentScreenshot, screen);
}

void clearScreenshot() {
    if (currentScreenshot)
        SDL_FreeSurface(currentScreenshot);
}