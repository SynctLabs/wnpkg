#include "lnpkg/file.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

int lnpkg_mkdir(char* dir) {
  return mkdir(dir, 0755);
}

int lnpkg_rmdir(char* dir) {
  return rmdir(dir);
}

int lnpkg_havedir(char* dir) {
  struct stat info;
  if (stat(dir, &info) != 0) {
    return 0;
  } else if (S_ISDIR(info.st_mode)) {
    return 1;
  } else {
    return 0;
  }
}

struct lnpkg_listdir_data* lnpkg_listdir(char* path) {
  struct lnpkg_listdir_data* tld =
      malloc(sizeof(struct lnpkg_listdir_data));
  tld->dirs = malloc(sizeof(struct lnpkg_file) *
                     LNPKG_FILE_LIST_DIR_INITIAL_CAPACITY);  // 100
  tld->len = 0;
  tld->capacity = LNPKG_FILE_LIST_DIR_INITIAL_CAPACITY;

  DIR* d;
  struct dirent* dir;

  d = opendir(path);

  if (d) {
    while ((dir = readdir(d)) != NULL) {
      if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
        continue;
      if (tld->len >= tld->capacity) {
        tld->capacity = tld->len + 1;
        tld->dirs = realloc(tld->dirs, sizeof(struct lnpkg_file) * tld->capacity);
      }
      size_t nlen = tld->len++;
      tld->dirs[nlen].filetype = (dir->d_type == DT_DIR) ? LNPKG_DIR : LNPKG_FILE;
      tld->dirs[nlen].name = strdup(dir->d_name);
      tld->dirs[nlen].rawdir = dir;
    }
    closedir(d);
  }

  return tld;
}

void lnpkg_listdir_close(struct lnpkg_listdir_data* tld) {
  for (size_t i = 0; i < tld->len; i++) {
    free(tld->dirs[i].name);
  }
  free(tld->dirs);
  free(tld);
}

void lnpkg_rmr(char* path) {
  if (lnpkg_havedir(path) == 1) {
    struct lnpkg_listdir_data* build_files = lnpkg_listdir(path);
    char fullpath[1024];
    for (size_t i = 0; i < build_files->len; ++i) {
      struct lnpkg_file f = build_files->dirs[i];
      char* name = f.name;
      if (f.filetype == LNPKG_FILE) {
        memset(fullpath, 0, 1024);
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, name);
        remove(fullpath);
      } else if (f.filetype == LNPKG_DIR) {
        memset(fullpath, 0, 1024);
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, name);
        lnpkg_rmr(fullpath);
      }
    }
    lnpkg_listdir_close(build_files);
    lnpkg_rmdir(path);
  }
}