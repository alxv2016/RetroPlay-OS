#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

#include "../common/defines.h"

/* System images, this module is used by launch.sh script to show and display
an image during boot. It can be launched via shell script by passing a
string of the image name */

static void blitImage(void *_dst, void *_src, int w, int h, int x, int y) {
  uint8_t *dst = (uint8_t *)_dst;
  uint8_t *src = (uint8_t *)_src;

  src += ((h * w) - 1) * 3;

  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      *(dst + 0) = *(src + 2); // r
      *(dst + 1) = *(src + 1); // g
      *(dst + 2) = *(src + 0); // b
      *(dst + 3) = 0xf;        // alpha
      dst += 4;
      src -= 3;
    }
  }
}

int main(int argc, char *argv[]) {

  if (argc < 2) {
    puts("Usage: show image.png");
    return 0;
  }

  char path[MAX_PATH];
  if (strchr(argv[1], '/') == NULL)
    sprintf(path, "/mnt/SDCARD/.system/res/%s", argv[1]);
  else
    strncpy(path, argv[1], MAX_PATH);

  if (access(path, F_OK) != 0)
    return 0; // nothing to show :(

  int fb0_fd = open("/dev/fb0", O_RDWR);
  struct fb_var_screeninfo vinfo;
  ioctl(fb0_fd, FBIOGET_VSCREENINFO, &vinfo);
  int map_size =
      vinfo.xres * vinfo.yres * (vinfo.bits_per_pixel / 8); // 640x480x4
  char *fb0_map =
      (char *)mmap(0, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, fb0_fd, 0);
  memset(fb0_map, 0, map_size); // clear screen

  SDL_Surface *img = IMG_Load(path); // 24-bit opaque png

  uint8_t *dst = (uint8_t *)fb0_map;     // rgba
  uint8_t *src = (uint8_t *)img->pixels; // bgr
  src += ((img->h * img->w) - 1) * 3;
  for (int y = 0; y < img->h; y++) {
    for (int x = 0; x < img->w; x++) {
      *(dst + 0) = *(src + 2); // r
      *(dst + 1) = *(src + 1); // g
      *(dst + 2) = *(src + 0); // b
      *(dst + 3) = 0xf;        // alpha
      dst += 4;
      src -= 3;
    }
  }

  SDL_FreeSurface(img);
  munmap(fb0_map, map_size);
  close(fb0_fd);

  return EXIT_SUCCESS;
}
