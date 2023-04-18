#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
// #include <dirent.h>
#include <unistd.h>
#include <ctype.h>

#include "defines.h"
#include "utils.h"

/* HELPERS */
static int replaceString(char *line, const char *search, const char *replace) {
  char *sp; // start of pattern
  if ((sp = strstr(line, search)) == NULL) {
    return 0;
  }
  int count = 1;
  int sLen = strlen(search);
  int rLen = strlen(replace);
  if (sLen > rLen) {
    // move from right to left
    char *src = sp + sLen;
    char *dst = sp + rLen;
    while ((*dst = *src) != '\0') {
      dst++;
      src++;
    }
  } else if (sLen < rLen) {
    // move from left to right
    int tLen = strlen(sp) - sLen;
    char *stop = sp + rLen;
    char *src = sp + sLen + tLen;
    char *dst = sp + rLen + tLen;
    while (dst >= stop) {
      *dst = *src;
      dst--;
      src--;
    }
  }
  memcpy(sp, replace, rLen);
  count += replaceString(sp + rLen, search, replace);
  return count;
}

char *escapeSingleQuotes(char *str) {
  replaceString(str, "'", "'\\''");
  return str;
}

int getIndexChar(char *str) {
  char i = 0;
  char c = tolower(str[0]);
  if (c >= 'a' && c <= 'z')
    i = (c - 'a') + 1;
  return i;
}

int prefixMatch(char *pre, char *str) {
  return (strncasecmp(pre, str, strlen(pre)) == 0);
}

int suffixMatch(char *suf, char *str) {
  int len = strlen(suf);
  int offset = strlen(str) - len;
  return (offset >= 0 && strncasecmp(suf, str + offset, len) == 0);
}

int exactMatch(char *str1, char *str2) {
  int len1 = strlen(str1);
  if (len1 != strlen(str2))
    return 0;
  return (strncmp(str1, str2, len1) == 0);
}

int hide(char *file_name) { return file_name[0] == '.'; }

void normalizeNewline(char *line) {
  int len = strlen(line);
  if (len > 1 && line[len - 1] == '\n' && line[len - 2] == '\r') { // windows!
    line[len - 2] = '\n';
    line[len - 1] = '\0';
  }
}

void trimTrailingNewlines(char *line) {
  int len = strlen(line);
  while (len > 0 && line[len - 1] == '\n') {
    line[len - 1] = '\0'; // trim newline
    len -= 1;
  }
}

int exists(char *path) { return access(path, F_OK) == 0; }

void touch(char *path) { close(open(path, O_RDWR | O_CREAT, 0777)); }

void putFile(char *path, char *contents) {
  FILE *file = fopen(path, "w");
  if (file) {
    fputs(contents, file);
    fclose(file);
  }
}

void getFile(char *path, char *buffer, size_t buffer_size) {
  FILE *file = fopen(path, "r");
  if (file) {
    fseek(file, 0L, SEEK_END);
    size_t size = ftell(file);
    if (size > buffer_size - 1)
      size = buffer_size - 1;
    rewind(file);
    fread(buffer, sizeof(char), size, file);
    fclose(file);
    buffer[size] = '\0';
  }
}

int getInt(char *path) {
  int i = 0;
  FILE *file = fopen(path, "r");
  if (file != NULL) {
    fscanf(file, "%i", &i);
    fclose(file);
  }
  return i;
}

void putInt(char *path, int value) {
  char buffer[8];
  sprintf(buffer, "%d", value);
  putFile(path, buffer);
}

void trimSortingMeta(char **str) { // eg. `001) `
  char *safe = *str;
  while (isdigit(**str))
    *str += 1; // ignore leading numbers

  if (*str[0] == ')') { // then match a closing parenthesis
    *str += 1;
  } else { //  or bail, restoring the string to its original value
    *str = safe;
    return;
  }

  while (isblank(**str))
    *str += 1; // ignore leading space
}

/* ARRAYS */

Array *Array_new(void) {
  Array *self = malloc(sizeof(Array));
  self->count = 0;
  self->capacity = 8;
  self->items = malloc(sizeof(void *) * self->capacity);
  return self;
}

void Array_push(Array *self, void *item) {
  if (self->count >= self->capacity) {
    self->capacity *= 2;
    self->items = realloc(self->items, sizeof(void *) * self->capacity);
  }
  self->items[self->count++] = item;
}

void Array_unshift(Array *self, void *item) {
  if (self->count == 0)
    return Array_push(self, item);
  Array_push(self, NULL); // ensures we have enough capacity
  for (int i = self->count - 2; i >= 0; i--) {
    self->items[i + 1] = self->items[i];
  }
  self->items[0] = item;
}

void *Array_pop(Array *self) {
  if (self->count == 0)
    return NULL;
  return self->items[--self->count];
}

void Array_reverse(Array *self) {
  int end = self->count - 1;
  int mid = self->count / 2;
  for (int i = 0; i < mid; i++) {
    void *item = self->items[i];
    self->items[i] = self->items[end - i];
    self->items[end - i] = item;
  }
}

void Array_free(Array *self) {
  free(self->items);
  free(self);
}

int StringArray_indexOf(Array *self, char *str) {
  for (int i = 0; i < self->count; i++) {
    if (exactMatch(self->items[i], str))
      return i;
  }
  return -1;
}

void StringArray_free(Array *self) {
  for (int i = 0; i < self->count; i++) {
    free(self->items[i]);
  }
  Array_free(self);
}

IntArray *IntArray_new(void) {
  IntArray *self = malloc(sizeof(IntArray));
  self->count = 0;
  memset(self->items, 0, sizeof(int) * INT_ARRAY_MAX);
  return self;
}

void IntArray_push(IntArray *self, int i) { self->items[self->count++] = i; }

void IntArray_free(IntArray *self) { free(self); }

/*****/

Hash *Hash_new(void) {
  Hash *self = malloc(sizeof(Hash));
  self->keys = Array_new();
  self->values = Array_new();
  return self;
}

void Hash_free(Hash *self) {
  StringArray_free(self->keys);
  StringArray_free(self->values);
  free(self);
}

void Hash_set(Hash *self, char *key, char *value) {
  Array_push(self->keys, strdup(key));
  Array_push(self->values, strdup(value));
}

char *Hash_get(Hash *self, char *key) {
  int i = StringArray_indexOf(self->keys, key);
  if (i == -1)
    return NULL;
  return self->values->items[i];
}

int isCharging(void) {
  // Code adapted from OnionOS
  char *cmd = "cd /customer/app/ ; ./axp_test";
  int batJsonSize = 100;
  char buf[batJsonSize];
  int charge_number;
  int result;

  FILE *fp;
  fp = popen(cmd, "r");
  if (fgets(buf, batJsonSize, fp) != NULL) {
    sscanf(buf, "{\"battery\":%*d, \"voltage\":%*d, \"charging\":%d}",
           &charge_number);
    result = (charge_number == 3);
  }
  pclose(fp);
  return result;
}