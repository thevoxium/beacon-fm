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
    use_default_colors();                                                      \
    init_pair(PAIR_SELECTED, COLOR_BLACK, COLOR_CYAN);                         \
    init_pair(PAIR_DIR, COLOR_CYAN, -1);                                       \
    init_pair(PAIR_FILE, COLOR_WHITE, -1);                                     \
  } while (0)

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) < (b) ? (b) : (a))

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
void update(Directory **directory, int key);

void copy_to_clipboard(const char *text);

#endif // !FM_H
