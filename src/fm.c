#include "fm.h"

#define MARGIN_X 2
#define INIT_CAPACITY 64

int init_ncurses(void) {
  if (initscr() == NULL) {
    return 1;
  }

  cbreak();
  noecho();
  curs_set(0);
  keypad(stdscr, TRUE);
  start_color();
  use_default_colors();

  init_pair(PAIR_SELECTED, COLOR_BLACK, COLOR_CYAN);
  init_pair(PAIR_DIR, COLOR_CYAN, -1);
  init_pair(PAIR_FILE, COLOR_WHITE, -1);

  return 0;
}

static const KeyActionTable key_actions_table_pairs[] = {
    {'y', ACTION_YANK}, {'h', ACTION_PARENT}, {'j', ACTION_DOWN},
    {'k', ACTION_UP},   {'l', ACTION_NEXT},   {'.', ACTION_HIDDEN_TOGGLE},
};

static Action action_from_key(int ch) {
  for (size_t i = 0;
       i < sizeof(key_actions_table_pairs) / sizeof(key_actions_table_pairs[0]);
       i++) {
    if (key_actions_table_pairs[i].key == ch)
      return key_actions_table_pairs[i].action;
  }

  return ACTION_NONE;
}

Directory *directory_init(bool show_hidden) {
  Directory *directory = (Directory *)malloc(sizeof(Directory));
  if (directory == NULL) {
    return NULL;
  }

  directory->path = getcwd(NULL, 0);
  if (directory->path == NULL) {
    perror("getcwd");
    free(directory);
    return NULL;
  }

  DIR *dp;
  dp = opendir(directory->path);
  if (dp == NULL) {
    perror("opendir");
    free(directory->path);
    free(directory);
    return NULL;
  }

  DirectoryEntry *entry;

  directory->show_hidden = show_hidden; // 0 means do not show
  directory->current_row = 0;
  directory->count = 0;
  directory->capacity = INIT_CAPACITY;
  directory->entries =
      (FileEntry *)malloc(directory->capacity * sizeof(FileEntry));
  if (directory->entries == NULL) {
    closedir(dp);
    free(directory->path);
    free(directory);
    return NULL;
  }

  while ((entry = readdir(dp)) != NULL) {
    if (directory->count == directory->capacity) {
      directory->capacity *= 2;
      FileEntry *resized = (FileEntry *)realloc(
          directory->entries, directory->capacity * sizeof(FileEntry));
      if (resized == NULL) {
        closedir(dp);
        directory_free(directory);
        return NULL;
      }
      directory->entries = resized;
    }

    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
      continue;
    }
    if (entry->d_name[0] == '.' && show_hidden == 0) {
      continue;
    }

    FileEntry *file = &directory->entries[directory->count];
    file->name = strdup(entry->d_name);
    if (file->name == NULL) {
      closedir(dp);
      directory_free(directory);
      return NULL;
    }
    file->type = entry->d_type;
    directory->count++;
  }

  closedir(dp);

  return directory;
}

void directory_free(Directory *directory) {
  if (directory == NULL)
    return;
  for (size_t i = 0; i < directory->count; i++)
    free(directory->entries[i].name);
  free(directory->entries);
  free(directory->path);
  free(directory);
}

void handle_keys(Directory **directory, int key) {
  Directory *dir = *directory;
  size_t current_row = dir->current_row;
  size_t count = dir->count;
  bool show_hidden = dir->show_hidden;
  const char *path = dir->path;
  Action action = action_from_key(key);

  switch (action) {
  case ACTION_YANK: {
    const char *name = dir->entries[current_row].name;
    size_t len = strlen(path) + 1 + strlen(name) + 1;
    char *full = (char *)malloc(len);
    if (full != NULL) {
      snprintf(full, len, "%s/%s", dir->path, name);
      copy_to_clipboard(full);
      free(full);
    }
    break;
  }

  case ACTION_PARENT:
    if (chdir("..") != 0)
      break;
    directory_free(dir);
    (*directory) = directory_init(show_hidden);
    break;

  case ACTION_DOWN:
    if (count > 0 && current_row < count - 1)
      dir->current_row++;
    break;

  case ACTION_UP:
    if (current_row > 0)
      dir->current_row--;
    break;

  case ACTION_NEXT:
    if (count == 0 || current_row >= count)
      break;
    if (dir->entries[current_row].type != DT_DIR)
      break;

    chdir(dir->entries[current_row].name);
    Directory *next_dir = directory_init(show_hidden);
    if (next_dir != NULL) {
      directory_free(dir);
      (*directory) = next_dir;
    }
    break;

  case ACTION_HIDDEN_TOGGLE:
    if (show_hidden == 0) {
      show_hidden = 1;
    } else {
      show_hidden = 0;
    }

    Directory *new_dir = directory_init(show_hidden);
    if (new_dir != NULL) {
      directory_free(dir);
      (*directory) = new_dir;
    }
    break;

  default:
    break;
  }
}

void render(Directory **directory) {
  int rows, cols;
  getmaxyx(stdscr, rows, cols);

  size_t visible = (size_t)rows;
  size_t first = ((*directory)->current_row >= visible)
                     ? ((*directory)->current_row - visible + 1)
                     : 0;

  for (size_t i = first; i < MIN((*directory)->count, first + visible); i++) {
    FileEntry *file = &(*directory)->entries[i];
    move((int)(i - first), MARGIN_X);
    if (i == (*directory)->current_row) {
      attron(COLOR_PAIR(PAIR_SELECTED));
      if (file->type == DT_DIR) {
        printw("> %s", file->name);
      } else {
        printw("%s", file->name);
      }
      attroff(COLOR_PAIR(PAIR_SELECTED));
    } else if (file->type == DT_DIR) {
      attron(COLOR_PAIR(PAIR_DIR) | A_BOLD);
      printw("> %s", file->name);
      attroff(COLOR_PAIR(PAIR_DIR) | A_BOLD);
    } else {
      attron(COLOR_PAIR(PAIR_FILE));
      printw("%s", file->name);
      attroff(COLOR_PAIR(PAIR_FILE));
    }
  }
}

void update(Directory **directory, int key) {
  clear();
  handle_keys(directory, key);
  render(directory);
}

void copy_to_clipboard(const char *text) {
  if (text == NULL) {
    return;
  }

#ifdef __APPLE__
  FILE *pipe = popen("pbcopy", "w");
  if (pipe == NULL) {
    return;
  }
  size_t len = strlen(text);
  if (fwrite(text, 1, len, pipe) != len) {
    pclose(pipe);
    return;
  }
  pclose(pipe);

  return;
#endif /* ifdef __APPLE */
}
