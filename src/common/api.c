#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

#include "defines.h"
#include "utils.h"

#include "api.h"

Directory *top;
// DirectoryArray
Array *stack;
// RecentArray
Array *recents;

int quit = 0;
int can_resume = 0;
// set to 1 on BTN_R1esume but only if can_resume==1
int should_resume = 0;
char slot_path[256];
int restore_depth = -1;
int restore_relative = -1;
int restore_selected = 0;
int restore_start = 0;
int restore_end = 0;

/*****/
void getDisplayName(const char *in_name, char *out_name) {
  char *tmp;
  strcpy(out_name, in_name);

  // extract just the filename if necessary
  tmp = strrchr(in_name, '/');
  if (tmp)
    strcpy(out_name, tmp + 1);

  // remove extension
  tmp = strrchr(out_name, '.');
  if (tmp && strlen(tmp) <= 4)
    tmp[0] = '\0'; // 3 letter extension plus dot

  // remove trailing parens (round and square)
  char safe_name[256];
  strcpy(safe_name, out_name);
  while ((tmp = strrchr(out_name, '(')) != NULL ||
         (tmp = strrchr(out_name, '[')) != NULL) {
    if (tmp == out_name)
      break;
    tmp[0] = '\0';
    tmp = out_name;
  }

  // make sure we haven't nuked the entire name
  if (out_name[0] == '\0')
    strcpy(out_name, safe_name);

  // remove trailing whitespace
  tmp = out_name + strlen(out_name) - 1;
  while (tmp > out_name && isspace((unsigned char)*tmp))
    tmp--;
  tmp[1] = '\0';
}

void getEmuName(const char *in_name, char *out_name) {
  char *tmp;
  strcpy(out_name, in_name);
  tmp = out_name;

  // extract just the Roms folder name if necessary
  if (prefixMatch(ROMS_PATH, tmp)) {
    tmp += strlen(ROMS_PATH) + 1;
    char *tmp2 = strchr(tmp, '/');
    if (tmp2)
      tmp2[0] = '\0';
  }

  // finally extract pak name from parenths if present
  tmp = strrchr(tmp, '(');
  if (tmp) {
    tmp += 1;
    strcpy(out_name, tmp);
    tmp = strchr(out_name, ')');
    tmp[0] = '\0';
  }
}

void getUniqueName(Entry *entry, char *out_name) {
  char *filename = strrchr(entry->path, '/') + 1;
  char emu_tag[256];
  getEmuName(entry->path, emu_tag);

  char *tmp;
  strcpy(out_name, entry->name);
  tmp = out_name + strlen(out_name);
  strcpy(tmp, " (");
  tmp = out_name + strlen(out_name);
  strcpy(tmp, emu_tag);
  tmp = out_name + strlen(out_name);
  strcpy(tmp, ")");
}

int hasEmu(char *emu_name) {
  // if (exactMatch(emu_name, "PAK")) return 1;
  char pak_path[256];
  sprintf(pak_path, "%s/Emus/%s.pak/launch.sh", PAKS_PATH, emu_name);
  if (exists(pak_path))
    return 1;

  sprintf(pak_path, "%s/Emus/%s.pak/launch.sh", SDCARD_PATH, emu_name);
  return exists(pak_path);
}

void getEmuPath(char *emu_name, char *pak_path) {
  sprintf(pak_path, "%s/Emus/%s.pak/launch.sh", SDCARD_PATH, emu_name);
  if (exists(pak_path))
    return;
  sprintf(pak_path, "%s/Emus/%s.pak/launch.sh", PAKS_PATH, emu_name);
}

int hasAlt(char *emu_name) {
  char pak_path[256];
  sprintf(pak_path, "%s/Emus/%s.pak/has-alt", PAKS_PATH, emu_name);
  return exists(pak_path);
}

int hasCue(char *dir_path, char *cue_path) {
  // NOTE: dir_path not rom_path
  char *tmp = strrchr(dir_path, '/') + 1; // folder name
  sprintf(cue_path, "%s/%s.cue", dir_path, tmp);
  return exists(cue_path);
}

int hasM3u(char *rom_path, char *m3u_path) {
  // NOTE: rom_path not dir_path
  char *tmp;

  strcpy(m3u_path, rom_path);
  tmp = strrchr(m3u_path, '/') + 1;
  tmp[0] = '\0';

  // path to parent directory
  char base_path[256];
  strcpy(base_path, m3u_path);

  tmp = strrchr(m3u_path, '/');
  tmp[0] = '\0';

  // get parent directory name
  char dir_name[256];
  tmp = strrchr(m3u_path, '/');
  strcpy(dir_name, tmp);

  // dir_name is also our m3u file name
  tmp = m3u_path + strlen(m3u_path);
  strcpy(tmp, dir_name);

  // add extension
  tmp = m3u_path + strlen(m3u_path);
  strcpy(tmp, ".m3u");

  return exists(m3u_path);
}

int hasCollections(void) {
  int has = 0;
  if (!exists(COLLECTIONS_PATH))
    return has;

  DIR *dh = opendir(COLLECTIONS_PATH);
  struct dirent *dp;
  while ((dp = readdir(dh)) != NULL) {
    if (hide(dp->d_name))
      continue;
    has = 1;
    break;
  }
  closedir(dh);
  return has;
}

int hasRoms(char *dir_name) {
  int has = 0;
  char emu_name[256];
  char rom_path[256];

  getEmuName(dir_name, emu_name);

  // check for emu pak
  if (!hasEmu(emu_name))
    return has;

  // check for at least one non-hidden file (we're going to assume it's a rom)
  sprintf(rom_path, "%s/%s/", ROMS_PATH, dir_name);
  DIR *dh = opendir(rom_path);
  if (dh != NULL) {
    struct dirent *dp;
    while ((dp = readdir(dh)) != NULL) {
      if (hide(dp->d_name))
        continue;
      has = 1;
      break;
    }
    closedir(dh);
  }
  // if (!has) printf("No roms for %s!\n", dir_name);
  return has;
}

int hasRecents(Array *recents) {
  int has = 0;

  Array *parent_paths = Array_new();
  if (exists(CHANGE_DISC_PATH)) {
    char sd_path[256];
    getFile(CHANGE_DISC_PATH, sd_path, 256);
    if (exists(sd_path)) {
      char *disc_path =
          sd_path + strlen(SDCARD_PATH); // makes path platform agnostic
      Recent *recent = Recent_new(disc_path);
      if (recent->available)
        has += 1;
      Array_push(recents, recent);

      char parent_path[256];
      strcpy(parent_path, disc_path);
      char *tmp = strrchr(parent_path, '/') + 1;
      tmp[0] = '\0';
      Array_push(parent_paths, strdup(parent_path));
    }
    unlink(CHANGE_DISC_PATH);
  }

  FILE *file = fopen(RECENT_PATH, "r"); // newest at top
  if (file) {
    char line[256];
    while (fgets(line, 256, file) != NULL) {
      normalizeNewline(line);
      trimTrailingNewlines(line);
      if (strlen(line) == 0)
        continue; // skip empty lines

      char sd_path[256];
      sprintf(sd_path, "%s%s", SDCARD_PATH, line);
      if (exists(sd_path)) {
        if (recents->count < MAX_RECENTS) {
          // this logic replaces an existing disc from a multi-disc game with
          // the last used
          char m3u_path[256];
          if (hasM3u(sd_path, m3u_path)) { // TODO: this might tank launch speed
            char parent_path[256];
            strcpy(parent_path, line);
            char *tmp = strrchr(parent_path, '/') + 1;
            tmp[0] = '\0';

            int found = 0;
            for (int i = 0; i < parent_paths->count; i++) {
              char *path = parent_paths->items[i];
              if (prefixMatch(path, parent_path)) {
                found = 1;
                break;
              }
            }
            if (found)
              continue;

            Array_push(parent_paths, strdup(parent_path));
          }
          Recent *recent = Recent_new(line);
          if (recent->available)
            has += 1;
          Array_push(recents, recent);
        }
      }
    }
    fclose(file);
  }
  saveRecents(recents);
  StringArray_free(parent_paths);
  return has > 0;
}

int isConsoleDir(char *path) {
  char *tmp;
  char parent_dir[256];
  strcpy(parent_dir, path);
  tmp = strrchr(parent_dir, '/');
  tmp[0] = '\0';

  return exactMatch(parent_dir, ROMS_PATH);
}

int getFirstDisc(char *m3u_path, char *disc_path) {
  // based on getDiscs() natch
  int found = 0;

  char base_path[256];
  strcpy(base_path, m3u_path);
  char *tmp = strrchr(base_path, '/') + 1;
  tmp[0] = '\0';

  FILE *file = fopen(m3u_path, "r");
  if (file) {
    char line[256];
    while (fgets(line, 256, file) != NULL) {
      normalizeNewline(line);
      trimTrailingNewlines(line);
      if (strlen(line) == 0)
        continue; // skip empty lines

      sprintf(disc_path, "%s%s", base_path, line);

      if (exists(disc_path))
        found = 1;
      break;
    }
    fclose(file);
  }
  return found;
}

void queueNext(char *cmd) {
  putFile("/tmp/next", cmd);
  quit = 1;
}

///
/*******/

Entry *Entry_new(char *path, int type) {
  char display_name[256];
  char emu_tag[256];
  getDisplayName(path, display_name);
  getEmuName(path, emu_tag);
  Entry *self = malloc(sizeof(Entry));
  self->path = strdup(path);
  self->name = strdup(display_name);
  self->emuTag = hasEmu(emu_tag)? strdup(emu_tag): "NA";
  self->unique = NULL;
  self->type = type;
  self->alpha = 0;
  self->has_alt = type != ENTRY_ROM ? 0 : -1;
  self->use_alt = type != ENTRY_ROM ? 0 : -1;
  return self;
}

void Entry_free(Entry *self) {
  free(self->path);
  free(self->name);
  if (hasEmu(self->emuTag))
    free(self->emuTag);
  if (self->unique)
    free(self->unique);
  free(self);
}

int EntryArray_indexOf(Array *self, char *path) {
  for (int i = 0; i < self->count; i++) {
    Entry *entry = self->items[i];
    if (exactMatch(entry->path, path))
      return i;
  }
  return -1;
}

int EntryArray_sortEntry(const void *a, const void *b) {
  Entry *item1 = *(Entry **)a;
  Entry *item2 = *(Entry **)b;
  return strcasecmp(item1->name, item2->name);
}

void EntryArray_sort(Array *self) {
  qsort(self->items, self->count, sizeof(void *), EntryArray_sortEntry);
}

void EntryArray_free(Array *self) {
  for (int i = 0; i < self->count; i++) {
    Entry_free(self->items[i]);
  }
  Array_free(self);
}

/*******/
// Goes before Directory functions
Array *getRoot(Array *recents) {
  Array *root = Array_new();
  if (hasRecents(recents))
    Array_push(root, Entry_new(FAUX_RECENT_PATH, ENTRY_DIR));

  DIR *dh;

  Array *entries = Array_new();
  dh = opendir(ROMS_PATH);
  if (dh != NULL) {
    struct dirent *dp;
    char *tmp;
    char full_path[256];
    sprintf(full_path, "%s/", ROMS_PATH);
    tmp = full_path + strlen(full_path);
    Array *emus = Array_new();
    while ((dp = readdir(dh)) != NULL) {
      if (hide(dp->d_name))
        continue;
      if (hasRoms(dp->d_name)) {
        strcpy(tmp, dp->d_name);
        Array_push(emus, Entry_new(full_path, ENTRY_DIR));
      } else {
        // Show Emulators
        strcpy(tmp, dp->d_name);
        Array_push(emus, Entry_new(full_path, ENTRY_DIR));
      }
    }
    EntryArray_sort(emus);
    Entry *prev_entry = NULL;
    for (int i = 0; i < emus->count; i++) {
      Entry *entry = emus->items[i];
      if (prev_entry != NULL) {
        if (exactMatch(prev_entry->name, entry->name)) {
          Entry_free(entry);
          continue;
        }
      }
      Array_push(entries, entry);
      prev_entry = entry;
    }
    Array_free(emus); // just free the array part, entries now owns emus entries
    closedir(dh);
  }

  if (hasCollections()) {
    if (entries->count)
      Array_push(root, Entry_new(COLLECTIONS_PATH, ENTRY_DIR));
    else { // no visible systems, promote collections to root
      dh = opendir(COLLECTIONS_PATH);
      if (dh != NULL) {
        struct dirent *dp;
        char *tmp;
        char full_path[256];
        sprintf(full_path, "%s/", COLLECTIONS_PATH);
        tmp = full_path + strlen(full_path);
        Array *collections = Array_new();
        while ((dp = readdir(dh)) != NULL) {
          if (hide(dp->d_name))
            continue;
          strcpy(tmp, dp->d_name);
          Array_push(
              collections,
              Entry_new(full_path,
                        ENTRY_DIR)); // yes, collections are fake directories
        }
        EntryArray_sort(collections);
        for (int i = 0; i < collections->count; i++) {
          Array_push(entries, collections->items[i]);
        }
        Array_free(collections); // just free the array part, entries now owns
                                 // collections entries
        closedir(dh);
      }
    }
  }

  // add systems to root
  for (int i = 0; i < entries->count; i++) {
    Array_push(root, entries->items[i]);
  }
  Array_free(entries); // root now owns entries' entries

  char tools_path[256];
  sprintf(tools_path, "%s/Apps", SDCARD_PATH);
  if (exists(tools_path))
    Array_push(root, Entry_new(tools_path, ENTRY_DIR));
  return root;
}

Array *getRecents(Array *recents) {
  Array *entries = Array_new();
  for (int i = 0; i < recents->count; i++) {
    Recent *recent = recents->items[i];
    if (!recent->available)
      continue;

    char sd_path[256];
    sprintf(sd_path, "%s%s", SDCARD_PATH, recent->path);
    int type = suffixMatch(".pak", sd_path) ? ENTRY_PAK : ENTRY_ROM; // ???
    Array_push(entries, Entry_new(sd_path, type));
  }
  return entries;
}

Array *getCollection(char *path) {
  Array *entries = Array_new();
  FILE *file = fopen(path, "r");
  if (file) {
    char line[256];
    while (fgets(line, 256, file) != NULL) {
      normalizeNewline(line);
      trimTrailingNewlines(line);
      if (strlen(line) == 0)
        continue; // skip empty lines

      char sd_path[256];
      sprintf(sd_path, "%s%s", SDCARD_PATH, line);
      if (exists(sd_path)) {
        int type = suffixMatch(".pak", sd_path) ? ENTRY_PAK : ENTRY_ROM; // ???
        Array_push(entries, Entry_new(sd_path, type));

        // char emu_name[256];
        // getEmuName(sd_path, emu_name);
        // if (hasEmu(emu_name)) {
        // Array_push(entries, Entry_new(sd_path, kEntryRom));
        // }
      }
    }
    fclose(file);
  }
  return entries;
}

Array *getDiscs(char *path) {
  // TODO: does path have SDCARD_PATH prefix?
  Array *entries = Array_new();

  char base_path[256];
  strcpy(base_path, path);
  char *tmp = strrchr(base_path, '/') + 1;
  tmp[0] = '\0';

  // TODO: limit number of discs supported (to 9?)
  FILE *file = fopen(path, "r");
  if (file) {
    char line[256];
    int disc = 0;
    while (fgets(line, 256, file) != NULL) {
      normalizeNewline(line);
      trimTrailingNewlines(line);
      if (strlen(line) == 0)
        continue; // skip empty lines

      char disc_path[256];
      sprintf(disc_path, "%s%s", base_path, line);

      if (exists(disc_path)) {
        disc += 1;
        Entry *entry = Entry_new(disc_path, ENTRY_ROM);
        free(entry->name);
        char name[16];
        sprintf(name, "Disc %i", disc);
        entry->name = strdup(name);
        Array_push(entries, entry);
      }
    }
    fclose(file);
  }
  return entries;
}

/*******/

void Directory_index(Directory *self) {
  int skip_index =
      exactMatch(FAUX_RECENT_PATH, self->path) ||
      prefixMatch(COLLECTIONS_PATH, self->path); // not alphabetized

  Entry *prior = NULL;
  int alpha = -1;
  int index = 0;
  for (int i = 0; i < self->entries->count; i++) {
    Entry *entry = self->entries->items[i];
    if (prior != NULL && exactMatch(prior->name, entry->name)) {
      if (prior->unique)
        free(prior->unique);
      if (entry->unique)
        free(entry->unique);

      char *prior_filename = strrchr(prior->path, '/') + 1;
      char *entry_filename = strrchr(entry->path, '/') + 1;
      if (exactMatch(prior_filename, entry_filename)) {
        char prior_unique[256];
        char entry_unique[256];
        getUniqueName(prior, prior_unique);
        getUniqueName(entry, entry_unique);

        prior->unique = strdup(prior_unique);
        entry->unique = strdup(entry_unique);
      } else {
        prior->unique = strdup(prior_filename);
        entry->unique = strdup(entry_filename);
      }
    }

    if (!skip_index) {
      int a = getIndexChar(entry->name);
      if (a != alpha) {
        index = self->alphas->count;
        IntArray_push(self->alphas, i);
        alpha = a;
      }
      entry->alpha = index;
    }

    prior = entry;
  }
}

Directory *Directory_new(char *path, int selected, Array *recents) {
  char display_name[256];
  char emu_tag[256];
  getDisplayName(path, display_name);
  getEmuName(path, emu_tag);

  Directory *self = malloc(sizeof(Directory));
  self->path = strdup(path);
  self->name = strdup(display_name);
  self->emuTag = hasEmu(emu_tag)? strdup(emu_tag): "NA";
  if (exactMatch(path, SDCARD_PATH)) {
    self->entries = getRoot(recents);
  } else if (exactMatch(path, FAUX_RECENT_PATH)) {
    self->entries = getRecents(recents);
  } else if (!exactMatch(path, COLLECTIONS_PATH) &&
             prefixMatch(COLLECTIONS_PATH, path)) {
    self->entries = getCollection(path);
  } else if (suffixMatch(".m3u", path)) {
    self->entries = getDiscs(path);
  } else {
    self->entries = getEntries(path);
  }
  self->alphas = IntArray_new();
  self->selected = selected;
  self->consoleDir = isConsoleDir(path);
  Directory_index(self);
  return self;
}

void Directory_free(Directory *self) {
  free(self->path);
  free(self->name);
  if (hasEmu(self->emuTag))
    free(self->emuTag);
  EntryArray_free(self->entries);
  IntArray_free(self->alphas);
  free(self);
}

void DirectoryArray_pop(Array *self) { Directory_free(Array_pop(self)); }

void DirectoryArray_free(Array *self) {
  for (int i = 0; i < self->count; i++) {
    Directory_free(self->items[i]);
  }
  Array_free(self);
}

Recent *Recent_new(char *path) {
  Recent *self = malloc(sizeof(Recent));

  char sd_path[256]; // only need to get emu name
  sprintf(sd_path, "%s%s", SDCARD_PATH, path);

  char emu_name[256];
  getEmuName(sd_path, emu_name);

  self->path = strdup(path);
  self->available = hasEmu(emu_name);
  return self;
}

void Recent_free(Recent *self) {
  free(self->path);
  free(self);
}

int RecentArray_indexOf(Array *self, char *str) {
  for (int i = 0; i < self->count; i++) {
    Recent *item = self->items[i];
    if (exactMatch(item->path, str))
      return i;
  }
  return -1;
}

void RecentArray_free(Array *self) {
  for (int i = 0; i < self->count; i++) {
    Recent_free(self->items[i]);
  }
  Array_free(self);
}

void saveRecents(Array *recents) {
  FILE *file = fopen(RECENT_PATH, "w");
  if (file) {
    for (int i = 0; i < recents->count; i++) {
      Recent *recent = recents->items[i];
      fputs(recent->path, file);
      putc('\n', file);
    }
    fclose(file);
  }
}

void addRecent(char *path, Array *recents) {
  path += strlen(SDCARD_PATH); // makes paths platform agnostic
  int id = RecentArray_indexOf(recents, path);
  if (id == -1) { // add
    while (recents->count >= MAX_RECENTS) {
      Recent_free(Array_pop(recents));
    }
    Array_unshift(recents, Recent_new(path));
  } else if (id > 0) { // bump to top
    for (int i = id; i > 0; i--) {
      void *tmp = recents->items[i - 1];
      recents->items[i - 1] = recents->items[i];
      recents->items[i] = tmp;
    }
  }
  saveRecents(recents);
}

/****/

Array *getEntries(char *path) {
  Array *entries = Array_new();

  if (isConsoleDir(path)) { 
    // top-level console folder, might collate
    char collated_path[256];
    strcpy(collated_path, path);
    char *tmp = strrchr(collated_path, '(');
    if (tmp) {
      tmp[1] = '\0'; // 1 because we want to keep the opening parenthesis to
                     // avoid collating "Game Boy Color" and "Game Boy Advance"
                     // into "Game Boy"

      DIR *dh = opendir(ROMS_PATH);
      if (dh != NULL) {
        struct dirent *dp;
        char full_path[256];
        sprintf(full_path, "%s/", ROMS_PATH);
        tmp = full_path + strlen(full_path);
        // while loop so we can collate paths, see above
        while ((dp = readdir(dh)) != NULL) {
          if (hide(dp->d_name))
            continue;
          if (dp->d_type != DT_DIR)
            continue;
          strcpy(tmp, dp->d_name);

          if (!prefixMatch(collated_path, full_path))
            continue;
          addEntries(entries, full_path);
        }
        closedir(dh);
      }
    }
  } else
    addEntries(entries, path); // just a subfolder
  EntryArray_sort(entries);
  return entries;
}

void addEntries(Array *entries, char *path) {
  DIR *dh = opendir(path);
  if (dh != NULL) {
    struct dirent *dp;
    char *tmp;
    char full_path[256];
    sprintf(full_path, "%s/", path);
    tmp = full_path + strlen(full_path);
    while ((dp = readdir(dh)) != NULL) {
      if (hide(dp->d_name))
        continue;
      strcpy(tmp, dp->d_name);
      int is_dir = dp->d_type == DT_DIR;
      int type;
      if (is_dir) {
        // TODO: this should make sure launch.sh exists
        if (suffixMatch(".pak", dp->d_name)) {
          type = ENTRY_PAK;
        } else {
          type = ENTRY_DIR;
        }
      } else {
        if (prefixMatch(COLLECTIONS_PATH, full_path)) {
          type = ENTRY_DIR; // :shrug:
        } else {
          type = ENTRY_ROM;
        }
      }
      Array_push(entries, Entry_new(full_path, type));
    }
    closedir(dh);
  }
}

int Entry_hasAlt(Entry *self) {
  // has_alt can be set by getEntries()
  // but won't be set by getRecents()
  // otherwise delayed until selected
  if (self->has_alt == -1) {
    // check
    char emu_name[256];
    getEmuName(self->path, emu_name);
    self->has_alt = hasAlt(emu_name);
  }
  return self->has_alt;
}

int Entry_useAlt(Entry *self) {
  // has to be checked on an individual basis
  // but delayed until selected

  if (self->use_alt == -1) {
    // check
    char emu_name[256];
    getEmuName(self->path, emu_name);

    char rom_name[256];
    char *tmp = strrchr(self->path, '/');
    if (tmp)
      strcpy(rom_name, tmp + 1);

    char use_alt[256];
    sprintf(use_alt, "%s/.mmenu/%s/%s.use-alt", USERDATA_PATH, emu_name,
            rom_name);

    self->use_alt = exists(use_alt);
  }
  return self->use_alt;
}

int Entry_toggleAlt(Entry *self) {
  if (!Entry_hasAlt(self))
    return 0;

  self->use_alt = !Entry_useAlt(self);

  char emu_name[256];
  getEmuName(self->path, emu_name);

  char rom_name[256];
  char *tmp = strrchr(self->path, '/');
  if (tmp)
    strcpy(rom_name, tmp + 1);

  char use_alt_path[256];
  sprintf(use_alt_path, "%s/.mmenu/%s/%s.use-alt", USERDATA_PATH, emu_name,
          rom_name);

  if (self->use_alt == 1)
    touch(use_alt_path);
  else
    unlink(use_alt_path);

  return 1;
}

// ROM Options
void readyResumePath(char *rom_path, int type) {
  char *tmp;
  can_resume = 0;
  char path[256];
  strcpy(path, rom_path);

  if (!prefixMatch(ROMS_PATH, path))
    return;

  char auto_path[256];
  if (type == ENTRY_DIR) {
    if (!hasCue(path, auto_path)) {      // no cue?
      tmp = strrchr(auto_path, '.') + 1; // extension
      strcpy(tmp, "m3u");                // replace with m3u
      if (!exists(auto_path))
        return; // no m3u
    }
    strcpy(path, auto_path); // cue or m3u if one exists
  }

  if (!suffixMatch(".m3u", path)) {
    char m3u_path[256];
    if (hasM3u(path, m3u_path)) {
      // change path to m3u path
      strcpy(path, m3u_path);
    }
  }

  char emu_name[256];
  getEmuName(path, emu_name);

  char rom_file[256];
  tmp = strrchr(path, '/') + 1;
  strcpy(rom_file, tmp);

  sprintf(slot_path, "%s/.mmenu/%s/%s.txt", USERDATA_PATH, emu_name,
          rom_file); // /.userdata/.mmenu/<EMU>/<romname>.ext.txt

  can_resume = exists(slot_path);
}

void readyResume(Entry *entry) { readyResumePath(entry->path, entry->type); }

int autoResume(void) {
  // NOTE: bypasses recents
  if (!exists(AUTO_RESUME_PATH))
    return 0;

  char path[256];
  getFile(AUTO_RESUME_PATH, path, 256);
  unlink(AUTO_RESUME_PATH);
  sync();

  // make sure rom still exists
  char sd_path[256];
  sprintf(sd_path, "%s%s", SDCARD_PATH, path);
  if (!exists(sd_path))
    return 0;

  // make sure emu still exists
  char emu_name[256];
  getEmuName(sd_path, emu_name);

  char emu_path[256];
  getEmuPath(emu_name, emu_path);

  if (!exists(emu_path))
    return 0;

  // putFile(kLastPath, Paths.fauxRecentDir); // saveLast() will crash here
  // because top is NULL

  char cmd[256];
  sprintf(cmd, "'%s' '%s'", escapeSingleQuotes(emu_path),
          escapeSingleQuotes(sd_path));
  putFile(RESUME_SLOT_PATH, AUTO_RESUME_SLOT);
  queueNext(cmd);
  return 1;
}

void openPak(char *path) {
  // NOTE: escapeSingleQuotes() modifies the passed string
  // so we need to save the path before we call that
  // if (prefixMatch(ROMS_PATH, path)) {
  // 	addRecent(path);
  // }
  saveLast(path);

  char cmd[256];
  sprintf(cmd, "'%s/launch.sh'", escapeSingleQuotes(path));
  queueNext(cmd);
}

void openRom(char *path, char *last) {
  char sd_path[256];
  strcpy(sd_path, path);

  char m3u_path[256];
  int has_m3u = hasM3u(sd_path, m3u_path);

  char recent_path[256];
  strcpy(recent_path, has_m3u ? m3u_path : sd_path);

  if (has_m3u && suffixMatch(".m3u", sd_path)) {
    getFirstDisc(m3u_path, sd_path);
  }

  char emu_name[256];
  getEmuName(sd_path, emu_name);

  if (should_resume) {
    char slot[16];
    getFile(slot_path, slot, 16);
    putFile(RESUME_SLOT_PATH, slot);
    should_resume = 0;

    if (has_m3u) {
      char rom_file[256];
      strcpy(rom_file, strrchr(m3u_path, '/') + 1);

      // get disc for state
      char disc_path_path[256];
      sprintf(disc_path_path, "%s/.mmenu/%s/%s.%s.txt", USERDATA_PATH, emu_name,
              rom_file, slot); // /.userdata/.mmenu/<EMU>/<romname>.ext.0.txt

      if (exists(disc_path_path)) {
        // switch to disc path
        char disc_path[256];
        getFile(disc_path_path, disc_path, 256);
        if (disc_path[0] == '/')
          strcpy(sd_path, disc_path); // absolute
        else {                        // relative
          strcpy(sd_path, m3u_path);
          char *tmp = strrchr(sd_path, '/') + 1;
          strcpy(tmp, disc_path);
        }
      }
    }
  } else
    putInt(RESUME_SLOT_PATH, 8); // resume hidden default state

  char emu_path[256];
  getEmuPath(emu_name, emu_path);

  // NOTE: escapeSingleQuotes() modifies the passed string
  // so we need to save the path before we call that
  addRecent(recent_path, recents);
  saveLast(last == NULL ? sd_path : last);

  char cmd[256];
  sprintf(cmd, "'%s' '%s'", escapeSingleQuotes(emu_path),
          escapeSingleQuotes(sd_path));
  queueNext(cmd);
}

void openDirectory(char *path, int auto_launch) {
  char auto_path[256];
  if (hasCue(path, auto_path) && auto_launch) {
    openRom(auto_path, path);
    return;
  }

  char m3u_path[256];
  strcpy(m3u_path, auto_path);
  char *tmp = strrchr(m3u_path, '.') + 1; // extension
  strcpy(tmp, "m3u");                     // replace with m3u
  if (exists(m3u_path) && auto_launch) {
    auto_path[0] = '\0';
    if (getFirstDisc(m3u_path, auto_path)) {
      openRom(auto_path, path);
      return;
    }
    // TODO: doesn't handle empty m3u files
  }

  int selected = 0;
  int start = selected;
  int end = 0;
  if (top && top->entries->count > 0) {
    if (restore_depth == stack->count && top->selected == restore_relative) {
      selected = restore_selected;
      start = restore_start;
      end = restore_end;
    }
  }

  top = Directory_new(path, selected, recents);
  top->start = start;
  top->end = end ? end
                 : ((top->entries->count < ROW_COUNT) ? top->entries->count
                                                      : ROW_COUNT);

  Array_push(stack, top);
}

void closeDirectory(void) {
  restore_selected = top->selected;
  restore_start = top->start;
  restore_end = top->end;
  DirectoryArray_pop(stack);
  restore_depth = stack->count;
  top = stack->items[stack->count - 1];
  restore_relative = top->selected;
}

void Entry_open(Entry *self) {
  if (self->type == ENTRY_ROM) {
    char *last = NULL;
    if (prefixMatch(COLLECTIONS_PATH, top->path)) {
      char *tmp;
      char filename[256];

      tmp = strrchr(self->path, '/');
      if (tmp)
        strcpy(filename, tmp + 1);

      char last_path[256];
      sprintf(last_path, "%s/%s", top->path, filename);
      last = last_path;
    }
    openRom(self->path, last);
  } else if (self->type == ENTRY_PAK) {
    openPak(self->path);
  } else if (self->type == ENTRY_DIR) {
    openDirectory(self->path, 1);
  }
}

void saveLast(char *path) {
  // special case for recently played
  if (exactMatch(top->path, FAUX_RECENT_PATH)) {
    // NOTE: that we don't have to save the file because
    // your most recently played game will always be at
    // the top which is also the default selection
    path = FAUX_RECENT_PATH;
  }
  putFile(LAST_PATH, path);
}

void loadLast(void) { // call after loading root directory
  if (!exists(LAST_PATH))
    return;

  char last_path[256];
  getFile(LAST_PATH, last_path, 256);

  char full_path[256];
  strcpy(full_path, last_path);

  char *tmp;
  char filename[256];
  tmp = strrchr(last_path, '/');
  if (tmp)
    strcpy(filename, tmp);

  Array *last = Array_new();
  while (!exactMatch(last_path, SDCARD_PATH)) {
    Array_push(last, strdup(last_path));

    char *slash = strrchr(last_path, '/');
    last_path[(slash - last_path)] = '\0';
  }

  while (last->count > 0) {
    char *path = Array_pop(last);
    if (!exactMatch(path, ROMS_PATH)) { // romsDir is effectively root as far as
                                        // restoring state after a game
      char collated_path[256];
      collated_path[0] = '\0';
      if (suffixMatch(")", path) && isConsoleDir(path)) {
        strcpy(collated_path, path);
        tmp = strrchr(collated_path, '(');
        if (tmp)
          tmp[1] = '\0'; // 1 because we want to keep the opening parenthesis to
                         // avoid collating "Game Boy Color" and "Game Boy
                         // Advance" into "Game Boy"
      }

      for (int i = 0; i < top->entries->count; i++) {
        Entry *entry = top->entries->items[i];

        // NOTE: strlen() is required for collated_path, '\0' wasn't reading as
        // NULL for some reason
        if (exactMatch(entry->path, path) ||
            (strlen(collated_path) &&
             prefixMatch(collated_path, entry->path)) ||
            (prefixMatch(COLLECTIONS_PATH, full_path) &&
             suffixMatch(filename, entry->path))) {
          top->selected = i;
          if (i >= top->end) {
            top->start = i;
            top->end = top->start + ROW_COUNT;
            if (top->end > top->entries->count) {
              top->end = top->entries->count;
              top->start = top->end - ROW_COUNT;
            }
          }
          if (last->count == 0 && !exactMatch(entry->path, FAUX_RECENT_PATH) &&
              !(!exactMatch(entry->path, COLLECTIONS_PATH) &&
                prefixMatch(COLLECTIONS_PATH, entry->path)))
            break; // don't show contents of auto-launch dirs

          if (entry->type == ENTRY_DIR) {
            openDirectory(entry->path, 0);
            break;
          }
        }
      }
    }
    free(path); // we took ownership when we popped it
  }

  StringArray_free(last);
}

void Menu_init(void) {
  stack = Array_new(); // array of open Directories
  recents = Array_new();

  openDirectory(SDCARD_PATH, 0);
  loadLast(); // restore state when available
}

void Menu_quit(void) {
  RecentArray_free(recents);
  DirectoryArray_free(stack);
}