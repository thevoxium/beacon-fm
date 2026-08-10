#include "fm.h"

int main(void) {
  INIT_NCURSES();

  Directory *directory = directory_init();
  if (directory == NULL) {
    endwin();
    return 1;
  }
  draw(directory);

  refresh();

  while (1) {
    int ch = getch();
    switch (ch) {
    case 'q':
    case 'Q':
      goto quit;
    default:
      continue;
    }
  }

quit:

  directory_free(directory);
  endwin();

  return 0;
}
