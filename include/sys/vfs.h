#include<sys/types.h>

#define true 1
#define false 0
#define MAX_LEN_FILE_NAME 20
#define MAX_CHILD 10

typedef uint8_t bool;
typedef struct v_file_node v_file_node;
typedef struct file_descriptor file_descriptor;

struct v_file_node {
	// Bounded Name and child size 
	char v_name[MAX_LEN_FILE_NAME];
	v_file_node* v_child[MAX_CHILD];
	v_file_node* parent;

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


v_file_node* root_node;

v_file_node* get_root_node();
v_file_node* search_file(char* dir_path, v_file_node * start_node);

//For Testing
void print_node_inorder(v_file_node* root);

