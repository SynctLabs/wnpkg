#include "wnpkg/file.h"

#include <sys/stat.h>

#ifdef WNPKG_WINDOWS
#include <direct.h>
#ifndef S_ISDIR
#define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#endif
#else
#include <unistd.h>
#endif

#include "wnpkg/plat.h"

int wnpkg_mkdir(char *dir)
{
#ifdef WNPKG_WINDOWS
  return _mkdir(dir);
#else
  return mkdir(dir, 0755);
#endif
}

int wnpkg_rmdir(char *dir)
{
#ifdef WNPKG_WINDOWS
  return _rmdir(dir);
#else
  return rmdir(dir);
#endif
}

int wnpkg_havedir(char *dir)
{
  struct stat info;
  if(stat(dir, &info) != 0) {
    return 0;
  } else if(S_ISDIR(info.st_mode)) {
    return 1;
  } else {
    return 0;
  }
}
