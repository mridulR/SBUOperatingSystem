#ifndef _DIRENT_S_H
#define _DIRENT_S_H

#include<sys/commons.h>

dir_info * sys_opendir(char *name);

struct dirent * sys_readdir(dir_info *dirp);

int closedir(dir_info *dirp);

dir_info * find_dir(uint64_t des);

bool add_dir(int curr_child_index, struct v_file_node * v_node);

bool delete_dir(uint64_t des);

void print_dir();

struct dirent * sys_readdir(dir_info *dirp);

int sys_open(const char *path, const char *mode);

int sys_close(int fd);

int sys_read(int fd, void *buf, int count);

#endif
