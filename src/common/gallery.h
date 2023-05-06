#ifndef _GALLERY_H
#define _GALLERY_H

int getScreenshotTotal(char *screenshotdir);
int loadScreenshots(char *screenshotDir, char ***imagePaths, int *totalPaths);
void showScreenshot(int selected, char **screenshots, SDL_Surface *screen);
void clearScreenshot();

#endif