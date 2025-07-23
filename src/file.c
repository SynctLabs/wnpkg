#include "wnpkg/file.h"

#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h>
#include <windows.h>

#ifndef S_ISDIR
#define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#endif
#define strdup _strdup
#else
#include <dirent.h>
#include <unistd.h>
#endif

#include "wnpkg/plat.h"

int wnpkg_mkdir(char *dir) {
#ifdef _WIN32
  return _mkdir(dir);
#else
  return mkdir(dir, 0755);
#endif
}

int wnpkg_rmdir(char *dir) {
#ifdef _WIN32
  return _rmdir(dir);
#else
  return rmdir(dir);
#endif
}

int wnpkg_havedir(char *dir) {
  struct stat info;
  if (stat(dir, &info) != 0) {
    return 0;
  } else if (S_ISDIR(info.st_mode)) {
    return 1;
  } else {
    return 0;
  }
}

struct wnpkg_listdir_data *wnpkg_listdir(char *path) {
  struct wnpkg_listdir_data *tld = malloc(sizeof(struct wnpkg_listdir_data));
  if (!tld) return NULL;

  tld->dirs = malloc(sizeof(struct wnpkg_file) * WNPKG_FILE_LIST_DIR_INITIAL_CAPACITY);
  if (!tld->dirs) {
    free(tld);
    return NULL;
  }

  tld->len = 0;
  tld->capacity = WNPKG_FILE_LIST_DIR_INITIAL_CAPACITY;

#ifdef _WIN32
  WIN32_FIND_DATA findFileData;
  char search_path[1024];
  snprintf(search_path, sizeof(search_path), "%s\\*", path);
  HANDLE hFind = FindFirstFile(search_path, &findFileData);

  if (hFind != INVALID_HANDLE_VALUE) {
    do {
      const char *name = findFileData.cFileName;
      if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
        continue;

      if (tld->len >= tld->capacity) {
        tld->capacity *= 2;
        tld->dirs = realloc(tld->dirs, sizeof(struct wnpkg_file) * tld->capacity);
        if (!tld->dirs) break;
      }

      size_t idx = tld->len++;
      tld->dirs[idx].name = strdup(name);
      tld->dirs[idx].filetype =
          (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? WNPKG_DIR : WNPKG_FILE;
    } while (FindNextFile(hFind, &findFileData) != 0);
    FindClose(hFind);
  }
#else
  DIR *d = opendir(path);
  struct dirent *dir;

  if (d) {
    while ((dir = readdir(d)) != NULL) {
      if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
        continue;

      if (tld->len >= tld->capacity) {
        tld->capacity *= 2;
        tld->dirs = realloc(tld->dirs, sizeof(struct wnpkg_file) * tld->capacity);
        if (!tld->dirs) break;
      }

      size_t idx = tld->len++;
      tld->dirs[idx].name = strdup(dir->d_name);
      tld->dirs[idx].filetype = (dir->d_type == DT_DIR) ? WNPKG_DIR : WNPKG_FILE;
    }
    closedir(d);
  }
#endif

  return tld;
}

void wnpkg_listdir_close(struct wnpkg_listdir_data *tld) {
  for (size_t i = 0; i < tld->len; i++) {
    free(tld->dirs[i].name);
  }
  free(tld->dirs);
  free(tld);
}

int wnpkg_rmdirr(char *path) {
  if (wnpkg_havedir(path) == 1) {
    struct wnpkg_listdir_data *build_files = wnpkg_listdir(path);
    char fullpath[1024];
    for (size_t i = 0; i < build_files->len; ++i) {
      struct wnpkg_file f = build_files->dirs[i];
      char *name = f.name;
      memset(fullpath, 0, sizeof(fullpath));
      snprintf(fullpath, sizeof(fullpath), "%s/%s", path, name);

      if (f.filetype == WNPKG_FILE) {
        remove(fullpath);
      } else if (f.filetype == WNPKG_DIR) {
        wnpkg_rmdirr(fullpath);
      }
    }
    wnpkg_listdir_close(build_files);
    wnpkg_rmdir(path);
  }
  return 0;
}
