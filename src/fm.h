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
  } while (0)

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
} Directory;

Directory *directory_init(void);
void directory_free(Directory *directory);
void draw(Directory *directory);

#endif // !FM_H
