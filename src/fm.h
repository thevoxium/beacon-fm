#ifndef FM_H
#define FM_H

#include <dirent.h>
#include <ncurses.h>
#include <stddef.h>
#include <stdint.h>

enum {
  PAIR_SELECTED = 1,
  PAIR_DIR,
  PAIR_FILE,
};

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) < (b) ? (b) : (a))

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

Directory *directory_init(void);
void directory_free(Directory *directory);
void handle_keys(Directory **directory, int key);
void render(Directory **directory, int key);
void update(Directory **directory, int key);

void copy_to_clipboard(const char *text);

#endif // !FM_H
