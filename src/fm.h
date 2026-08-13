#ifndef FM_H
#define FM_H

#include <dirent.h>
#include <locale.h>
#include <ncurses.h>
#include <stdbool.h>
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
  STATUS_BAR,
  PAIR_BG,
};

enum {
  GRUVBOX_BG = 8,
  GRUVBOX_FG,
  GRUVBOX_YELLOW,
  GRUVBOX_BLUE,
  GRUVBOX_GREEN,
  GRUVBOX_RED,
  GRUVBOX_ORANGE,
  GRUVBOX_AQUA,
  GRUVBOX_PURPLE,
  GRUVBOX_GRAY,
  GRUVBOX_BG_ALT,
};

typedef enum Action {
  ACTION_YANK,
  ACTION_PARENT,
  ACTION_NEXT,
  ACTION_UP,
  ACTION_DOWN,
  ACTION_NONE,
  ACTION_HIDDEN_TOGGLE,
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
  bool show_hidden;
} Directory;

Action key_to_action(int key);

Directory *directory_init(bool show_hidden);
void directory_free(Directory *directory);
void handle_keys(Directory **directory, int key);
void render(Directory *directory);
void update(Directory **directory, int key);

void copy_to_clipboard(const char *text);

#endif // !FM_H
