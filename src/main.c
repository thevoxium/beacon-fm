#include "fm.h"

int main(void) {
  INIT_NCURSES();

  Directory *directory = directory_init();
  if (directory == NULL) {
    endwin();
    return 1;
  }

  update(&directory, 0);
  refresh();

  while (1) {
    int ch = getch();
    if (ch == 'q') {
      break;
    }
    update(&directory, ch);
    refresh();
  }

  directory_free(directory);
  endwin();

  return 0;
}
