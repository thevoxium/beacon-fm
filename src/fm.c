#include "fm.h"

#define MARGIN_X 2
#include <dirent.h>
#include <ncurses.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

Directory *directory_init(void) {
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

  directory->current_row = 0;
  directory->count = 0;
  directory->capacity = 64;
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

    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
      continue;

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

void update(Directory **directory, KeyType key) {
  clear();
  Directory *dir = *directory;
  switch (key) {
  case H:
    if (chdir("..") != 0)
      break;
    directory_free(dir);
    (*directory) = directory_init();
    break;
  case J:
    if (dir->count > 0 && dir->current_row < dir->count - 1)
      dir->current_row++;
    break;
  case K:
    if (dir->current_row > 0)
      dir->current_row--;
    break;
  case L:
    if (dir->count == 0 || dir->current_row >= dir->count)
      break;
    if (dir->entries[dir->current_row].type != DT_DIR)
      break;

    chdir(dir->entries[dir->current_row].name);
    Directory *next_dir = directory_init();
    if (next_dir != NULL) {
      directory_free(dir);
      (*directory) = next_dir;
    }
    break;
  default:
    break;
  }

  if ((*directory) == NULL)
    return;

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
