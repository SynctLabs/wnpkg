#include "wnpkg/file.h"

#include <sys/stat.h>
#include <unistd.h>

int wnpkg_mkdir(char* dir) {
  return mkdir(dir, 0755);
}

int wnpkg_rmdir(char* dir) {
  return rmdir(dir);
}

int wnpkg_havedir(char* dir) {
  struct stat info;
  if (stat(dir, &info) != 0) {
    return 0;
  } else if (S_ISDIR(info.st_mode)) {
    return 1;
  } else {
    return 0;
  }
}