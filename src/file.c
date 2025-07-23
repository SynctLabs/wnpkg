#include "wnpkg/file.h"

#ifdef WNPKG_WINDOWS
#include <direct.h>
#else
#include <sys/stat.h>
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
  } else if(info.st_mode & __S_IFDIR) {
    return 1;
  } else {
    return 0;
  }
}