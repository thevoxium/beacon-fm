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

void draw(Directory *directory) {
  if (directory == NULL)
    return;
  for (size_t i = 0; i < directory->count; i++) {
    printw("%s %u\n", directory->entries[i].name,
           (unsigned int)directory->entries[i].type);
  }
}
