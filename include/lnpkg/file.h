#ifndef __FILE_H__
#define __FILE_H__

#include <stdio.h>
#include <stdlib.h>

#include <dirent.h>

#define LNPKG_FILE_LIST_DIR_INITIAL_CAPACITY 1

enum lnpkg_type { LNPKG_FILE, LNPKG_DIR };

struct lnpkg_file {
  struct dirent* rawdir;
  char* name;
  enum lnpkg_type filetype;
};

struct lnpkg_listdir_data {
  struct lnpkg_file* dirs;
  size_t len;
  size_t capacity;
};

int lnpkg_mkdir(char*);
int lnpkg_rmdir(char*);
int lnpkg_havedir(char*);
void lnpkg_rmr(char*);

struct lnpkg_listdir_data* lnpkg_listdir(char*);
void lnpkg_listdir_close(struct lnpkg_listdir_data*);

#endif