#ifndef __FILE_H__
#define __FILE_H__

#include <stdio.h>
#include <stdlib.h>

#include <dirent.h>

#define WNPKG_FILE_LIST_DIR_INITIAL_CAPACITY 1

enum wnpkg_type { WNPKG_FILE, WNPKG_DIR };

struct wnpkg_file {
  struct dirent* rawdir;
  char* name;
  enum wnpkg_type filetype;
};

struct wnpkg_listdir_data {
  struct wnpkg_file* dirs;
  size_t len;
  size_t capacity;
};

int wnpkg_mkdir(char*);
int wnpkg_rmdir(char*);
int wnpkg_havedir(char*);
int wnpkg_rmdirr(char*);

struct wnpkg_listdir_data* wnpkg_listdir(char*);
void wnpkg_listdir_close(struct wnpkg_listdir_data*);

#endif