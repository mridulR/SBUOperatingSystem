#include<sys/tarfs.h>
#include<sys/vfs.h>
#include<sys/kprintf.h>
#include<sys/kmalloc.h>
#include<sys/memcpy.h>
#include<sys/memset.h>
#include<sys/kstring.h>

#define PAGE_SIZE 4096


v_file_node* get_root_node() {
	return root_node; 
}


void print_node_inorder(v_file_node* root) {
	
	if (root == NULL) {
		return;
	}

    kprintf("File Name = %s, File Type isDir = %d, parent = %s , no_of_child = %d\n", 
		root->v_name, root->is_dir,	root->parent == NULL ? "NULL" : root->parent->v_name, root->no_of_child);	

	for (uint64_t i = 0; i < root->no_of_child; i++) {
	v_file_node *child = root->v_child[i];
	if (child->is_dir == false) {
		kprintf("File Name = %s, File Type isDir = %d, parent = %s, no_of_child = %d\n",
			 child->v_name, child->is_dir, child->parent == NULL ? "NULL" : child->parent->v_name,
			 child->no_of_child);
     } else {
		 print_node_inorder(child);
	   }
	}
}

v_file_node * build_Node(char * name, v_file_node * parent, int8_t is_dir, uint64_t start, uint64_t end) {
	v_file_node * curr = (v_file_node *)kmalloc(sizeof(v_file_node));
    memset((uint8_t *)curr, '\0', PAGE_SIZE);	
	memcpy(curr->v_name, name, kstrlen(name));
	curr->parent = parent;
	curr->is_dir = is_dir;
    curr->no_of_child = 0;
	curr->start_addr = start;
    curr->end_addr = end;    	

	return curr;
}

void insert_node_by_name(char *name, bool is_dir, uint64_t start_addr, uint64_t end_addr) {
	char *path = (char *) kmalloc(sizeof(char) * kstrlen(name));
    memset((uint8_t *)path, '\0', PAGE_SIZE);	
	memcpy(path, name, kstrlen(name));

	char *remaining = path;
	char *temp = kstr_tok(path, '/', &remaining);
    
	int done = 0;
    while ((done == 0) && (temp != NULL) && (*temp != '\0')) {
		int flag_child_of_directory = 0;
		v_file_node * trav_node = root_node->v_child[0];
		for (int index = 0; index < trav_node->no_of_child; index++) {
			if (kstrcmp(temp, trav_node->v_child[index]->v_name) == 0) {
				flag_child_of_directory = 1;
				break;
			}
			trav_node = (v_file_node *)trav_node->v_child[index];
		}
		if(flag_child_of_directory == 0) {
			while ((temp != NULL) &&  (*temp != '\0')) {
				v_file_node * curr_node = build_Node(temp, trav_node, 1, 0, 0);
                trav_node->v_child[trav_node->no_of_child] = curr_node;
                trav_node->no_of_child = trav_node->no_of_child + 1;
                trav_node = curr_node; 
                temp = kstr_tok(remaining, '/', &remaining);
            }
		trav_node->start_addr = start_addr;
		trav_node->end_addr = end_addr;
        trav_node->is_dir = is_dir;
        done = 1;
    }
	temp = kstr_tok(remaining, '/', &remaining);
	}
	kfree(path);
}


int32_t atoi(char *ch) {
	int num = 0;
	int sign = 1;
   
    if (*ch == '-') {
		sign = -1;
    }
	ch++;
    
	while(*ch) {
		if ((int)*ch >= 48 && (int)*ch <= 57) {
			num = (num << 3) + (num << 1) + (*ch) - '0';
			ch++;
		} else {
			return 0;
		}
    }

	return sign * num;

}

int32_t pow(int base, int power) {
	int i =0, product = 1;
	for (i = 0; i < power; ++i) {
		product = product * base;
	}
	return product;
}

int32_t oct_to_dec(int num) {
	int decimal = 0, index = 0, remainder;
	while (num != 0) {
		remainder = num % 10;
		num /= 10;
		decimal += remainder + pow(8, index);
		++index;
	}
	return decimal;
}


void* init_tarfs() {
    root_node = build_Node("/", NULL, true, 0, 0);
	v_file_node * mount_node = build_Node("rootfs", root_node, true, 0, 0);
    root_node->no_of_child += 1;
	root_node->v_child[0] = mount_node;

	posix_header_ustar* start = (posix_header_ustar*) & _binary_tarfs_start;


	int file_size = 0;
	while(start < (posix_header_ustar*) & _binary_tarfs_end) {
		file_size = oct_to_dec(atoi(start->size));

		if (kstrcmp(start->typeflag, "5") == 0) {
			insert_node_by_name(start->name, true, 0, 0);
		} else {
			insert_node_by_name(start->name, false, (uint64_t) (start + 1),
				 (uint64_t) ((void *)start + 512 + file_size));
        }

		if (file_size > 0) {
			kprintf("Listing Non-zero File Name - %s\n", start->name);
			start = start + (file_size / (sizeof(start) + 1)) + 2;
        } else {
			kprintf("Listing zero size File Name - %s\n", start->name);
			start = start + 1;
        }
    } 	
	return (void *)root_node ;
}


v_file_node* search_file(char* dir_path, v_file_node * start_node) {
    if (start_node == NULL) {
		return NULL;
	}

	if (dir_path == NULL || (*dir_path == '\0')) {
		return NULL;
	}


	char *path = (char *) kmalloc(sizeof(char) * kstrlen(dir_path));
    memset((uint8_t *)path, '\0', PAGE_SIZE);   
    memcpy(path, dir_path, kstrlen(dir_path));

    char *remaining = path;
    char *temp = kstr_tok(path, '/', &remaining);

	if (kstrcmp(temp, start_node->v_name) != 0) {
		kfree(path);
        return NULL;
    } else {
		if (remaining == NULL || *remaining == '\0') {
			return start_node;
		}
	}

	if (remaining == NULL || *remaining == '\0') {
		kfree(path);
		return NULL;
	}

	temp = kstr_tok(remaining, '/', &remaining);

	for (int index = 0; index < start_node->no_of_child; index++) {
        if (kstrcmp(temp, start_node->v_child[index]->v_name) == 0) {
			if (remaining == NULL || *remaining == '\0') {
				return start_node->v_child[index];
			}
			temp = kstr_tok(remaining, '/', &remaining);
			v_file_node * child = start_node->v_child[index];
			for (int ch = 0; ch < child->no_of_child; ch++) {
				v_file_node * rslt = search_file(temp, child->v_child[ch]);
				if (rslt != NULL) {
					return rslt;
				}
			}
            return NULL;
        } else {
			return NULL;
        }
	}
	kfree(path);
	return NULL;
}


