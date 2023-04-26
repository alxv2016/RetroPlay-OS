#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

#include "../common/defines.h"

#define STOCK_FONT "/customer/app/Helvetica-Neue-2.ttf"

/* System message, this module is used by the MainUI.sh and launch.sh
scripts to show messages during installation, updates and other messages.
A string is pass through the shell script and gets picked up here.
sys-img and confirm have similar logic */
float headingLineHeight = H1 * HEADING_LINEHEIGHT;

static void blit(void *_dst, int dst_w, int dst_h, void *_src, int src_w,
                 int src_h, int ox, int oy) {
  uint8_t *dst = (uint8_t *)_dst;
  uint8_t *src = (uint8_t *)_src;

  oy += headingLineHeight - src_h;

  for (int y = 0; y < src_h; y++) {
    uint8_t *dst_row = dst + (((((dst_h - 1 - oy) - y) * dst_w) - 1 - ox) * 4);
    uint8_t *src_row = src + ((y * src_w) * 4);
    for (int x = 0; x < src_w; x++) {
      float a = *(src_row + 3) / 255.0;
      if (a > 0.1) {
        *(dst_row + 0) = *(src_row + 0) * a;
        *(dst_row + 1) = *(src_row + 1) * a;
        *(dst_row + 2) = *(src_row + 2) * a;
        *(dst_row + 3) = 0xff;
      }
      dst_row -= 4;
      src_row += 4;
    }
  }
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    puts("Usage: say \"message\"");
    return EXIT_SUCCESS;
  }

  char *path = NULL;
  if (access(FONT_PATH, F_OK) == 0)
    path = FONT_PATH;
  else if (access(STOCK_FONT, F_OK) == 0)
    path = STOCK_FONT;
  if (!path)
    return 0;

  char str[MAX_PATH];
  strncpy(str, argv[1], MAX_PATH);

  int fb0_fd = open("/dev/fb0", O_RDWR);
  struct fb_var_screeninfo vinfo;
  ioctl(fb0_fd, FBIOGET_VSCREENINFO, &vinfo);
  int map_size =
      vinfo.xres * vinfo.yres * (vinfo.bits_per_pixel / 8); // 640x480x4
  char *fb0_map =
      (char *)mmap(0, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, fb0_fd, 0);
  // clear screen
  memset(fb0_map, 0, map_size);
  // INIT TEXT MESSAGE
  TTF_Init();
  TTF_Font *font = TTF_OpenFont(path, H1);

  int width = SCREEN_WIDTH;
  int height = SCREEN_HEIGHT;
  SDL_Color font_color = {WHITE};
  SDL_Surface *text;
  char *rows[MAX_ROW];
  int row_count = 0;

  char *tmp;
  rows[row_count++] = str;
  while ((tmp = strchr(rows[row_count - 1], '\n')) != NULL) {
    if (row_count + 1 >= MAX_ROW)
      break;
    rows[row_count++] = tmp + 1;
  }

  int rendered_height = headingLineHeight * row_count;
  int y = (height - rendered_height) / 2;

  char line[MAX_PATH];

  for (int i = 0; i < row_count; i++) {
    int len;
    if (i + 1 < row_count) {
      len = rows[i + 1] - rows[i] - 1;
      if (len)
        strncpy(line, rows[i], len);
      line[len] = '\0';
    } else {
      len = strlen(rows[i]);
      strcpy(line, rows[i]);
    }

    if (len) {
      text = TTF_RenderUTF8_Blended(font, line, font_color);
      int x = (width - text->w) / 2;
      blit(fb0_map, width, height, text->pixels, text->w, text->h, x, y);
      SDL_FreeSurface(text);
    }
    y += headingLineHeight;
  }

  TTF_CloseFont(font);
  TTF_Quit();
  // END OF MESSAGE
  munmap(fb0_map, map_size);
  close(fb0_fd);

  return EXIT_SUCCESS;
}
