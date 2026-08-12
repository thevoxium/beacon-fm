#include "fm.h"

int main(void) {

  if (init_ncurses() != 0) {
    return 1;
  }

  Directory *directory = directory_init(false);
  if (directory == NULL) {
    endwin();
    return 1;
  }

  update(&directory, ACTION_NONE);
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
