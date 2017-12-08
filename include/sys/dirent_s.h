#ifndef _DIRENT_S_H
#define _DIRENT_S_H

#include<sys/commons.h>

dir_info * opendir(char *name);
dir_info * find_dir(uint64_t des);
bool add_dir(int curr_child_index, struct v_file_node * v_node);
bool delete_dir(uint64_t des);
void print_dir();

struct dirent *readdir(dir_info *dirp);
int closedir(dir_info *dirp);


int open(const char *pathname, int flags);
int close(int fd);
int read(int fd, void *buf, int count);

#endif
