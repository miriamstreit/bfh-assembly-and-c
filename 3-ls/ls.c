#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

// define functions outside of main()
int printEntry(struct dirent *entry);
int filterScan(const struct dirent *);
int isDirectory(const char *path);

int main(int argc, char **argv) {
  char *fileOrDir = calloc(1, sizeof(char));
  if (fileOrDir == NULL) return EXIT_FAILURE;
  if (argc == 2) {
    fileOrDir = argv[1];
  } else {
    fileOrDir = ".";
  }

  if (isDirectory(fileOrDir) != 0) {
    struct dirent **entries;
    int filesCount = scandir(fileOrDir, &entries, *filterScan, alphasort);
    for (int e = 0; e < filesCount; e++) {
      printEntry(entries[e]);
    }
    free(entries);
  } else {
    printf("%s\n", fileOrDir);
  }

  return EXIT_SUCCESS;
}

int isDirectory(const char *path) {
  struct stat statbuf;
  if (stat(path, &statbuf) != 0) return EXIT_SUCCESS;
  return S_ISDIR(statbuf.st_mode);
}

int printEntry(struct dirent *entry) {
  struct stat fileStat;
  stat(entry->d_name, &fileStat);

  if (entry->d_type == DT_DIR) {
    printf("%s/\n", entry->d_name);
    return EXIT_SUCCESS;
  }
  if (entry->d_type == DT_LNK) {
    printf("%s@\n", entry->d_name);
    return EXIT_SUCCESS;
  }
  if (fileStat.st_mode & S_IXUSR) {
    printf("%s*\n", entry->d_name);
    return EXIT_SUCCESS;
  }

  printf("%s\n", entry->d_name);  
  return EXIT_SUCCESS;
}

int filterScan(const struct dirent *entry) {
  if ( (strcmp(entry->d_name, ".") == 0) | (strcmp(entry->d_name, "..") == 0) ) return 0;
  return 1;
}
