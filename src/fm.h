#ifndef FM_H
#define FM_H

#include <dirent.h>
#include <ncurses.h>
#include <stddef.h>
#include <stdint.h>

#define INIT_NCURSES()                                                         \
  do {                                                                         \
    if (initscr() == NULL) {                                                   \
      return 1;                                                                \
    }                                                                          \
    cbreak();                                                                  \
    noecho();                                                                  \
    curs_set(0);                                                               \
    keypad(stdscr, TRUE);                                                      \
    start_color();                                                             \
    init_pair(1, COLOR_WHITE, COLOR_GREEN);                                    \
  } while (0)

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) < (b) ? (b) : (a))

typedef enum {
  H,
  J,
  K,
  L,
  NONE,
} KeyType;

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
void update(Directory **directory, KeyType key);

#endif // !FM_H
