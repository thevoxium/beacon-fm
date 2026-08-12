#ifndef FM_H
#define FM_H

#include <dirent.h>
#include <ncurses.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) < (b) ? (b) : (a))

enum {
  PAIR_SELECTED = 1,
  PAIR_DIR,
  PAIR_FILE,
};

typedef enum Action {
  ACTION_YANK,
  ACTION_PARENT,
  ACTION_NEXT,
  ACTION_UP,
  ACTION_DOWN,
  ACTION_NONE,
} Action;

typedef struct KeyActionTable {
  int key;
  Action action;
} KeyActionTable;

int init_ncurses(void);

typedef struct dirent DirectoryEntry;

typedef struct FileEntry {
  char *name;
  uint8_t type;
} FileEntry;

typedef struct Directory {
  char *path;
  FileEntry *entries;
  size_t count;
  size_t capacity;
  size_t current_row;
} Directory;

Action key_to_action(int key);

Directory *directory_init(void);
void directory_free(Directory *directory);
void handle_keys(Directory **directory, int key);
void render(Directory **directory);
void update(Directory **directory, int key);

void copy_to_clipboard(const char *text);

#endif // !FM_H
