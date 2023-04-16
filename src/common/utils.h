#ifndef _UTILS_H
#define _UTILS_H

#define INT_ARRAY_MAX 27

typedef struct Array {
  int count;
  int capacity;
  void **items;
} Array;

typedef struct IntArray {
  int count;
  int items[INT_ARRAY_MAX];
} IntArray;

typedef struct Hash {
  Array *keys;
  Array *values;
} Hash; // not really a hash

int getIndexChar(char *str);
char *escapeSingleQuotes(char *str);
int getIndexChar(char *str);
int prefixMatch(char *pre, char *str);
int suffixMatch(char *suf, char *str);
int exactMatch(char *str1, char *str2);
int hide(char *file_name);
void normalizeNewline(char *line);
void trimTrailingNewlines(char *line);
int exists(char *path);
void touch(char *path);
void putFile(char *path, char *contents);
void getFile(char *path, char *buffer, size_t buffer_size);
int getInt(char *path);
void putInt(char *path, int value);
void trimSortingMeta(char **str);

/* ARRAYS */
Array *Array_new(void);
void Array_push(Array *self, void *item);
void Array_unshift(Array *self, void *item); 
void *Array_pop(Array *self); 
void Array_reverse(Array *self); // static
void Array_free(Array *self);
int StringArray_indexOf(Array *self, char *str); // static
void StringArray_free(Array *self);
IntArray *IntArray_new(void);
void IntArray_push(IntArray *self, int i); 
void IntArray_free(IntArray *self); 

/* HASHS */
Hash *Hash_new(void); // static
void Hash_free(Hash *self); // static
void Hash_set(Hash *self, char *key, char *value); // static
char *Hash_get(Hash *self, char *key); // static

#endif