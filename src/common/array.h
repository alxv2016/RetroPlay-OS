#ifndef _ARRAY_H
#define _ARRAY_H

#define INT_ARRAY_MAX 27
#define MAX_RECENTS 24

typedef struct Array {
  int count;
  int capacity;
  void **items;
} Array;

typedef struct Hash {
  Array *keys;
  Array *values;
} Hash; // not really a hash

/******/

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

/*****/

typedef struct IntArray {
  int count;
  int items[INT_ARRAY_MAX];
} IntArray;

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

/*****/
/*****/
/* UTILS */
int hasEmu(char *emu_name);
void getEmuPath(char *emu_name, char *pak_path);
int hasAlt(char *emu_name);
int hasCue(char *dir_path, char *cue_path);
int hasM3u(char *rom_path, char *m3u_path);
void queueNext(char *cmd);
char *escapeSingleQuotes(char *str);
int getIndexChar(char *str);
void getUniqueName(Entry *entry, char *out_name);

/* ARRAYS */
Array *Array_new(void);
void Array_push(Array *self, void *item);
void Array_unshift(Array *self, void *item);
void *Array_pop(Array *self);
void Array_reverse(Array *self);
void Array_free(Array *self);
int StringArray_indexOf(Array *self, char *str);
void StringArray_free(Array *self);
IntArray *IntArray_new(void);
void IntArray_push(IntArray *self, int i);
void IntArray_free(IntArray *self);

/* HASHS */
Hash *Hash_new(void);
void Hash_free(Hash *self);
void Hash_set(Hash *self, char *key, char *value);
char *Hash_get(Hash *self, char *key);

/* ENTRY */

Entry *Entry_new(char *path, int type);
void Entry_free(Entry *self);
int EntryArray_indexOf(Array *self, char *path);
int EntryArray_sortEntry(const void *a, const void *b);
void EntryArray_sort(Array *self);
void EntryArray_free(Array *self);
int Entry_hasAlt(Entry *self);
int Entry_useAlt(Entry *self);
int Entry_toggleAlt(Entry *self);
void Entry_open(Entry *self);

/* Directory */
void Directory_index(Directory *self);
Directory *Directory_new(char *path, int selected);
void Directory_free(Directory *self);
void DirectoryArray_pop(Array *self);
void DirectoryArray_free(Array *self);

/* RECENT */
Recent *Recent_new(char *path);
void Recent_free(Recent *self);
int RecentArray_indexOf(Array *self, char *str);
void RecentArray_free(Array *self);

void saveRecents(void);
void addRecent(char *path);
int hasRecents(void);

/* COLLECTIONS */
int hasCollections(void);
int hasRoms(char *dir_name);

Array *getRoot(void);
Array *getRecents(void);
Array *getCollection(char *path);
Array *getDiscs(char *path);
int getFirstDisc(char *m3u_path, char *disc_path) ;
void addEntries(Array *entries, char *path);
Array *getEntries(char *path);
void readyResumePath(char *rom_path, int type);
void readyResume(Entry *entry);
int autoResume(void);
void openPak(char *path);
void openRom(char *path, char *last);
void openDirectory(char *path, int auto_launch);
void closeDirectory(void);
void saveLast(char *path);
void loadLast(void);

void Menu_init(void);
void Menu_quit(void);

#endif