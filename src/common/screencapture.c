#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <dirent.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <png.h>
#include <msettings.h>
#include <SDL/SDL_mixer.h>

#include "defines.h"
#include "../common/rumble.h"

#include "screencapture.h"

// Search pid of running executable
pid_t searchpid(const char *commname) {
  DIR *procdp;
  struct dirent *dir;
  char fname[32];
  char comm[128];
  pid_t pid;
  pid_t ret = 0;

  procdp = opendir("/proc");
  while ((dir = readdir(procdp))) {
    if (dir->d_type == DT_DIR) {
      pid = atoi(dir->d_name);
      if (pid > 2) {
        sprintf(fname, "/proc/%d/comm", pid);
        FILE *fp = fopen(fname, "r");
        if (fp) {
          fscanf(fp, "%127s", comm);
          fclose(fp);
          if (!strcmp(comm, commname)) {
            ret = pid;
            break;
          }
        }
      }
    }
  }
  closedir(procdp);
  return ret;
}

// Get Most recent file name from Roms/recentlist.json
char *getrecent(char *filename) {
  FILE *fp;
  char *fnptr;
  uint32_t i;

  strcpy(filename, "/mnt/SDCARD/Screenshots/");
  if (access(filename, F_OK))
    mkdir(filename, 777);

  fnptr = filename + strlen(filename);

  if (!access("/tmp/cmd_to_run.sh", F_OK)) {
    // for stock
    if ((fp = fopen("/mnt/SDCARD/Roms/recentlist.json", "r"))) {
      fscanf(fp, "%*255[^:]%*[:]%*[\"]%255[^\"]", fnptr);
      fclose(fp);
    }
  } else if ((fp = fopen("/tmp/next", "r"))) {
    // for RetroPlay OS
    char ename[256];
    char fname[256];
    char *strptr;
    ename[0] = 0;
    fname[0] = 0;
    fscanf(fp, "%*[\"]%255[^\"]%*[\" ]%255[^\"]", ename, fname);
    fclose(fp);
    if (fname[0]) {
      if ((strptr = strrchr(fname, '.')))
        *strptr = 0;
      if ((strptr = strrchr(fname, '/')))
        strptr++;
      else
        strptr = fname;
      strcpy(fnptr, strptr);
    } else if (ename[0]) {
      if ((strptr = strrchr(ename, '/')))
        *strptr = 0;
      if ((strptr = strrchr(ename, '.')))
        *strptr = 0;
      if ((strptr = strrchr(ename, '/')))
        strptr++;
      else
        strptr = ename;
      strcpy(fnptr, strptr);
    }
  }

  if (!(*fnptr)) {
    if (searchpid("retroplay"))
      strcat(filename, "retroplay");
    else
      strcat(filename, "MainUI");
  }

  fnptr = filename + strlen(filename);
  for (i = 0; i < 1000; i++) {
    sprintf(fnptr, "_%03d.png", i);
    if (access(filename, F_OK) != 0)
      break;
  }
  if (i > 999)
    return NULL;
  return filename;
}

// Screenshot (640x480, rotate180, png)
void screenshot(void) {
  char screenshotname[512];
  uint32_t *buffer, *src;
  uint32_t linebuffer[640], x, y, pix;
  FILE *fp;
  int fd_fb;
  struct fb_var_screeninfo vinfo;
  png_structp png_ptr;
  png_infop info_ptr;

  if (getrecent(screenshotname) == NULL)
    return;
  if ((buffer = (uint32_t *)malloc(640 * 480 * 4)) != NULL) {
    fd_fb = open("/dev/fb0", O_RDWR);
    ioctl(fd_fb, FBIOGET_VSCREENINFO, &vinfo);
    lseek(fd_fb, 640 * vinfo.yoffset * 4, SEEK_SET);
    read(fd_fb, buffer, 640 * 480 * 4);
    close(fd_fb);

    fp = fopen(screenshotname, "wb");
    if (fp != NULL) {
      png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
      info_ptr = png_create_info_struct(png_ptr);
      png_init_io(png_ptr, fp);
      png_set_IHDR(png_ptr, info_ptr, 640, 480, 8, PNG_COLOR_TYPE_RGBA,
                   PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
                   PNG_FILTER_TYPE_DEFAULT);
      png_write_info(png_ptr, info_ptr);
      src = buffer + 640 * 480;
      for (y = 0; y < 480; y++) {
        for (x = 0; x < 640; x++) {
          pix = *--src;
          linebuffer[x] = 0xFF000000 | (pix & 0x0000FF00) |
                          (pix & 0x00FF0000) >> 16 | (pix & 0x000000FF) << 16;
        }
        png_write_row(png_ptr, (png_bytep)linebuffer);
      }
      png_write_end(png_ptr, info_ptr);
      png_destroy_write_struct(&png_ptr, &info_ptr);
      fclose(fp);
      sync();
      superShortPulse();
    }
    free(buffer);
  }
}
