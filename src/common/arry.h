#ifndef _ARRY_H
#define _ARRY_H

#include "utils.h"

// a multiple of all menu rows
#define MAX_RECENTS 24 

enum EntryType {
  ENTRY_DIR,
  ENTRY_PAK,
  ENTRY_ROM,
};

typedef struct Entry {
  char *path;
  char *name;
  char *unique;
  int type;
  int alpha;
  int has_alt;
  int use_alt;
} Entry;

typedef struct Directory {
  char *path;
  char *name;
  Array *entries;
  IntArray *alphas;
  // rendering
  int selected;
  int start;
  int end;
} Directory;

typedef struct Recent {
  // NOTE: this is without the SDCARD_PATH prefix!
  char *path;
  int available;
} Recent;

extern Directory *top;
// DirectoryArray
extern Array *stack;
// RecentArray
extern Array *recents;

extern int quit;
extern int can_resume;
// set to 1 on BTN_R1esume but only if can_resume==1
extern int should_resume; 
extern char slot_path[256];

extern int restore_depth;
extern int restore_relative;
extern int restore_selected;
extern int restore_start;
extern int restore_end;

/*****/
void getDisplayName(const char *in_name, char *out_name);
void getEmuName(const char *in_name, char *out_name);
void getUniqueName(Entry *entry, char *out_name); //static
int hasEmu(char *emu_name); // static
void getEmuPath(char *emu_name, char *pak_path); // static
int hasAlt(char *emu_name); // static
int hasCue(char *dir_path, char *cue_path); // static
int hasM3u(char *rom_path, char *m3u_path); // static
int hasCollections(void); // static
int hasRoms(char *dir_name); // static
int hasRecents(Array *recents); // static
int isConsoleDir(char *path); // static
int getFirstDisc(char *m3u_path, char *disc_path);
void queueNext(char *cmd); // static

/* ENTRY */
Entry *Entry_new(char *path, int type); // static
void Entry_free(Entry *self); // static
int EntryArray_indexOf(Array *self, char *path); // static
int EntryArray_sortEntry(const void *a, const void *b); // static
void EntryArray_sort(Array *self); // static
void EntryArray_free(Array *self); // static

/* BEFORE DIRECTORY */
Array *getRoot(Array *recents); // static
Array *getRecents(Array *recents); // static
Array *getCollection(char *path); // static
Array *getDiscs(char *path); // static


/* Directory */
void Directory_index(Directory *self); // static
Directory *Directory_new(char *path, int selected, Array *recents);
void Directory_free(Directory *self); // static
void DirectoryArray_pop(Array *self); // static
void DirectoryArray_free(Array *self); // static

Recent *Recent_new(char *path); // static
void Recent_free(Recent *self); // static
int RecentArray_indexOf(Array *self, char *str); // static
void RecentArray_free(Array *self); // static
void saveRecents(Array *recents); // static
void addRecent(char *path, Array *recents);

Array *getEntries(char *path); // static
void addEntries(Array *entries, char *path); // static
int Entry_hasAlt(Entry *self); // static
int Entry_useAlt(Entry *self); // static
int Entry_toggleAlt(Entry *self); // static

void readyResumePath(char *rom_path, int type); // static
void readyResume(Entry *entry);
int autoResume(void);
void openPak(char *path); // static
void openRom(char *path, char *last); // static
void openDirectory(char *path, int auto_launch); // static
void closeDirectory(void);
void Entry_open(Entry *self);
void saveLast(char *path); // static
void loadLast(void); // static

void Menu_init(void);
void Menu_quit(void);

#endif