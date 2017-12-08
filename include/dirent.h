#ifndef _DIRENT_H
#define _DIRENT_H

#define NAME_MAX 255

#include <sys/commons.h>

/*struct dirent {
 char d_name[NAME_MAX+1];
};*/

typedef struct dir_info dir_info;

dir_info *opendir(const char *name);
struct dirent *readdir(dir_info *dirp);
int closedir(dir_info *dirp);

#endif
