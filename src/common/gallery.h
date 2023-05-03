#ifndef _GALLERY_H
#define _GALLERY_H

#define STR_MAX 256

int loadImagesPathsFromDir(const char *dir_path, char ***images_paths, int *images_paths_count);
char *drawImageByIndex(const int new_image_index, const int image_index, char **images_paths, const int images_paths_count, SDL_Surface *screen, SDL_Rect *frame, int *cache_used);
void cleanImagesCache();
int getImagesCount(const char *dir_path);

#endif