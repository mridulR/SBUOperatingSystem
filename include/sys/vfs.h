#ifndef _DIRENT_S_H
#define _DIRENT_S_H

#include<sys/commons.h>

v_file_node* root_node;
v_file_node* tarfs_mount_node;

v_file_node* get_root_node();
v_file_node* search_file(const char* dir_path, v_file_node * start_node);

//For Testing
void print_node_inorder(v_file_node* root);


#endif
