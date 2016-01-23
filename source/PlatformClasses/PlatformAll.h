#ifndef _PLATFORM_ALL
#define _PLATFORM_ALL

#include <stdio.h>  /* defines FILENAME_MAX */
#ifdef WIN32
#include <direct.h>
#define _GetCurrentDir _getcwd
#define _ChangeDir _chdir
#else
#include <dirent.h>
#include <unistd.h>
#define _GetCurrentDir getcwd
#define _ChangeDir chdir
#endif

#include <iostream>
#include <cstring>
#include <string>
#include <vector>

#ifndef WIN32
std::string GetCurrentDir();
std::vector<std::string> ListAllDirectories(std::string& directory);
void ChangeParentDir(std::string targetParent);
#endif
#endif //_PLATFORM_ALL
