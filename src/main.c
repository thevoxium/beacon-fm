#include "fm.h"

int main(void) {
  INIT_NCURSES();

  Directory *directory = directory_init();
  if (directory == NULL) {
    endwin();
    return 1;
  }

  update(&directory, NONE);
  refresh();

  while (1) {
    int ch = getch();
    switch (ch) {
    case 'q':
    case 'Q':
      goto quit;
    case 'l':
    case 'L':
      update(&directory, L);
      break;
    case 'j':
    case 'J':
      update(&directory, J);
      break;
    case 'k':
    case 'K':
      update(&directory, K);
      break;
    default:
      continue;
    }

    refresh();
  }
quit:

  directory_free(directory);
  endwin();

  return 0;
}
