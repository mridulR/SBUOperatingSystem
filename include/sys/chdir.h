#ifndef _CHDIR_H
#define _CHDIR_H


int sys_chdir(const char *path);
char * sys_getcwd(char *buf, int size);

#endif
