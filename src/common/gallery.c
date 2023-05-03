#include <ctype.h>
#include <dirent.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "gallery.h"

static SDL_Surface *g_image_cache_prev = NULL;
static SDL_Surface *g_image_cache_current = NULL;
static SDL_Surface *g_image_cache_next = NULL;

static const char *getFilenameExt(const char *filename) {
  const char *dot = strrchr(filename, '.');
  if (!dot || dot == filename)
    return "";
  return dot + 1;
}

static char *toLower(char *s) {
  for (char *p = s; *p; p++) {
    *p = tolower(*p);
  }
  return s;
}

static int getImagePath(const char *dir_path, const struct dirent *ent, char *image_path) {
  const int ext_size = 50;
  char ext[ext_size];
  const char *filename = ent->d_name;
  if (filename[0] == '.' || S_ISDIR(ent->d_type & DT_DIR)) {
    return 0;
  }
  strncpy(ext, getFilenameExt(filename), ext_size);
  const char *fileExt = toLower(ext);
  if (strcmp(fileExt, "png") == 0 || strcmp(fileExt, "jpg") == 0 ||
      strcmp(fileExt, "jpeg") == 0) {
    char full_path[PATH_MAX];
    sprintf(full_path, "%s%s", dir_path, filename);
    strcpy(image_path, full_path);
    return 1;
  }
  return 0;
}

int getImagesCount(const char *dir_path) {
  int images_count = 0;

  DIR *dir = opendir(dir_path);
  if (dir == NULL) {
    return 0;
  }

  struct dirent *ent;
  while ((ent = readdir(dir)) != NULL) {
    char image_path[PATH_MAX];
    const int is_image = getImagePath(dir_path, ent, image_path);
    if (is_image) {
      images_count++;
    }
  }
  closedir(dir);
  return images_count;
}

static int compare_strings(const void *a, const void *b) {
  const char *aa = *(const char **)a;
  const char *bb = *(const char **)b;
  return strcmp(aa, bb);
}

int loadImagesPathsFromDir(const char *dir_path, char ***images_paths, int *images_paths_count) {
  char normalized_dir_path[PATH_MAX];
  const int dir_path_length = strlen(dir_path);
  if (dir_path_length == 0) {
    return 0;
  }
  if (dir_path[dir_path_length - 1] != '/') {
    sprintf(normalized_dir_path, "%s/", dir_path);
  } else {
    strcpy(normalized_dir_path, dir_path);
  }

  const int images_count = getImagesCount(normalized_dir_path);

  DIR *dir = opendir(normalized_dir_path);

  if (dir == NULL) {
    return 0;
  }

  struct dirent *ent;

  *images_paths_count = 0;
  *images_paths = (char **)malloc(images_count * sizeof(char *));

  while ((ent = readdir(dir)) != NULL) {
    char image_path[PATH_MAX];
    const int is_image = getImagePath(normalized_dir_path, ent, image_path);

    if (!is_image) {
      continue;
    }

    (*images_paths)[*images_paths_count] =
        (char *)malloc(PATH_MAX * sizeof(char));
    strcpy((*images_paths)[*images_paths_count], image_path);
    (*images_paths_count)++;

    if ((*images_paths_count) >= images_count) {
      // we found more images than allocated memory
      // TODO: handle this
      break;
    }
  }

  closedir(dir);

  qsort(*images_paths, *images_paths_count, sizeof(char *), compare_strings);

  return 1;
}

/* IMAGE CACHE */
static void drawImage(SDL_Surface *image_to_draw, SDL_Surface *screen, const SDL_Rect *frame) {
  if (!image_to_draw)
    return;

  int border_left = 0;
  SDL_Rect new_frame = {0, 0};
  if (frame != NULL) {
    new_frame = *frame;
    border_left = new_frame.x;
  }
  int16_t image_x = 320 - (int16_t)(image_to_draw->w / 2);
  if (image_x < border_left) {
    image_x = border_left;
  } else {
    new_frame.x -= border_left;
  }
  SDL_Rect image_rect = {image_x, (int16_t)(240 - image_to_draw->h / 2)};
  if (frame != NULL)
    SDL_BlitSurface(image_to_draw, &new_frame, screen, &image_rect);
  else
    SDL_BlitSurface(image_to_draw, NULL, screen, &image_rect);
}

char *drawImageByIndex(const int new_image_index, const int image_index, char **images_paths, const int images_paths_count, SDL_Surface *screen, SDL_Rect *frame, int *cache_used) {
  if (new_image_index < 0 || new_image_index >= images_paths_count) {
    // out of range, draw nothing
    printf("out of range, draw nothing\n");
    return NULL;
  }
  char *image_path_to_draw = images_paths[new_image_index];
  if (new_image_index == image_index) {
    if (g_image_cache_current == NULL) {
      g_image_cache_prev = new_image_index == 0 ? NULL : IMG_Load(images_paths[new_image_index - 1]);
      g_image_cache_current = IMG_Load(images_paths[new_image_index]);
      g_image_cache_next = new_image_index == images_paths_count - 1 ? NULL : IMG_Load(images_paths[new_image_index + 1]);

      drawImage(g_image_cache_current, screen, frame);

      *cache_used = 0;
      return image_path_to_draw;
    }
  }
  if (abs(new_image_index - image_index) > 1) {
    return NULL;
  }

  int move_direction = new_image_index - image_index;

  if (move_direction > 0) {
    if (g_image_cache_prev)
      SDL_FreeSurface(g_image_cache_prev);
    g_image_cache_prev = g_image_cache_current;
    g_image_cache_current = g_image_cache_next;
    if (new_image_index == images_paths_count - 1) {
      g_image_cache_next = NULL;
    } else {
      const int next_image_index = new_image_index + 1;
      char *image_path_to_load = images_paths[next_image_index];
      g_image_cache_next = IMG_Load(image_path_to_load);
    }
    *cache_used = 1;
  } else if (move_direction < 0) {
    if (g_image_cache_next)
      SDL_FreeSurface(g_image_cache_next);
    g_image_cache_next = g_image_cache_current;
    g_image_cache_current = g_image_cache_prev;
    if (new_image_index == 0) {
      g_image_cache_prev = NULL;
    } else {
      const int prev_image_index = new_image_index - 1;
      char *image_path_to_load = images_paths[prev_image_index];
      g_image_cache_prev = IMG_Load(image_path_to_load);
    }

    *cache_used = 1;
  } else {
    *cache_used = 1;
  }

  drawImage(g_image_cache_current, screen, frame);

  return image_path_to_draw;
}

void cleanImagesCache() {
    if (g_image_cache_prev)
        SDL_FreeSurface(g_image_cache_prev);
    if (g_image_cache_current)
        SDL_FreeSurface(g_image_cache_current);
    if (g_image_cache_next)
        SDL_FreeSurface(g_image_cache_next);
}