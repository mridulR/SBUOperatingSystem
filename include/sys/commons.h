#ifndef _COMMON_H_
#define _COMMON_H_

#include <sys/vma.h>
#include<sys/types.h>

#define true 1
#define false 0
#define NAME_MAX 255
#define MAX_LEN_FILE_NAME 20
#define MAX_CHILD 10


typedef uint8_t bool;

struct v_file_node {
	// Bounded Name and child size 
	char v_name[MAX_LEN_FILE_NAME];
	struct v_file_node* v_child[MAX_CHILD];
	struct v_file_node* parent;

	bool is_dir;
	uint64_t inode_no;

	// File Management
	uint64_t start_addr;
	uint64_t curr_addr;
	uint64_t end_addr;

	// Metadata
	uint64_t no_of_child;
};


struct file_descriptor {
	v_file_node* v_node;
	uint64_t des_no;
};

typedef struct file_descriptor file_descriptor;

typedef enum File_Type {
	DIRECTORY_,
	FILE_
}File_Type;

struct dirent {
 char d_name[NAME_MAX+1];
};

struct dir_info {
 uint64_t des;
 int curr_child_index;
 struct v_file_node * v_node;
 struct dir_info * next;
 struct dir_info * prev;
};

#endif
