#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/inotify.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <msettings.h>
#include <pthread.h>

#include "../common/defines.h"
#include "../common/utils.h"

#include "../common/interface.h"

#define UPDATE_TXT "update_progress.txt"
#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN (1024 * (EVENT_SIZE + 16))

GFX gfx;

int main(int argc, char *argv[])
{
	int fd = inotify_init();
	int wd = inotify_add_watch(fd, TEMP_PATH, IN_MODIFY | IN_CREATE);

	SDL_Init(SDL_INIT_VIDEO);
	SDL_ShowCursor(0);
	InitSettings();

	gfx.screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 16, SDL_HWSURFACE | SDL_DOUBLEBUF);

	SDL_Surface *progress_bar_empty = loadImage("progress-bar-empty.png");
	SDL_Surface *progress_bar_full = loadImage("progress-bar-full.png");

	GFX_init();
	GFX_ready();

	int done = 0;
	int progress = -1;
	char msg[256];
	void updateProgress(void)
	{
		char new_msg[256];
		getFile(TEMP_PATH UPDATE_TXT, new_msg, 256);

		trimTrailingNewlines(new_msg);
		// printf("\t[%s]\n", new_msg); fflush(stdout);

		if (strlen(new_msg) == 0)
			return;

		if (exactMatch(new_msg, "quit"))
		{
			done = 1;
			return;
		}

		if (exactMatch(new_msg, msg))
			return;
		char *tmp = strchr(new_msg, ' ');
		// TODO: this doesn't do what I think it will (update message without updating progress bar)
		if (tmp)
		{
			tmp[0] = '\0';
			progress = atoi(new_msg);
			if (progress > 100)
				progress = 100;
			strcpy(msg, tmp + 1);
		}
		else
		{
			strcpy(msg, new_msg);
		}

		// printf("%f: %s\n", progress/100.0, msg); fflush(stdout);

		SDL_FillRect(gfx.screen, NULL, 0);
		if (progress > -1)
		{
			SDL_BlitSurface(progress_bar_empty, NULL, gfx.screen, &(SDL_Rect){120, 236});
			SDL_BlitSurface(progress_bar_full, &(SDL_Rect){0, 0, progress * 4, 8}, gfx.screen, &(SDL_Rect){120, 236});
		}
		paragraph(g_font.small, msg, 24, gfx.screen, NULL);
		SDL_Flip(gfx.screen);
	}

	if (exists(TEMP_PATH UPDATE_TXT))
		updateProgress(); // don't miss the first message

	while (!done)
	{

		struct inotify_event *event;
		char buffer[BUF_LEN];
		int length = read(fd, buffer, BUF_LEN); // blocks

		for (int i = 0; i < length; i += EVENT_SIZE + event->len)
		{
			event = (struct inotify_event *)&buffer[i];

			if (!event->len)
				continue;
			if (!(event->mask & IN_CREATE) && !(event->mask & IN_MODIFY))
				continue;
			if (!exactMatch(event->name, UPDATE_TXT))
				continue;

			updateProgress();
		}
	}

	inotify_rm_watch(fd, wd);
	close(fd);

	unlink(TEMP_PATH UPDATE_TXT);

	SDL_FillRect(gfx.screen, NULL, 0);
	SDL_Flip(gfx.screen);

	SDL_FreeSurface(progress_bar_empty);
	SDL_FreeSurface(progress_bar_full);

	GFX_quit();
	SDL_Quit();
	QuitSettings();

	return 0;
}