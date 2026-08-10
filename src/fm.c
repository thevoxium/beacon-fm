#include "fm.h"
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

  directory->current_row = 2;
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
  case J:
    if (dir->count > 0 && dir->current_row < dir->count - 1)
      dir->current_row++;
    break;
  case K:
    if (dir->current_row > 2)
      dir->current_row--;
    break;
  case L:
    if (dir->count == 0 || dir->current_row >= dir->count)
      break;
    if (dir->entries[dir->current_row].type != DT_DIR)
      break;
    if (chdir(dir->entries[dir->current_row].name) != 0)
      break;
    directory_free(dir);
    (*directory) = directory_init();
    break;
  default:
    break;
  }

  if ((*directory) == NULL)
    return;
  for (size_t i = 2; i < (*directory)->count; i++) {
    if (i == (size_t)(*directory)->current_row) {
      attron(COLOR_PAIR(1));
      printw("%s %u\n", (*directory)->entries[i].name,
             (unsigned int)(*directory)->entries[i].type);
      attroff(COLOR_PAIR(1));
    } else {
      printw("%s %u\n", (*directory)->entries[i].name,
             (unsigned int)(*directory)->entries[i].type);
    }
  }
}
