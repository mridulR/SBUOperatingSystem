#include <sys/types.h>
#include <sys/commons.h>
#include <sys/vfs.h>
#include <sys/dirent_s.h>
#include <sys/kern_process.h>
#include <sys/kprintf.h>
#include <sys/kmalloc.h>
#include <sys/memset.h>

#define PAGE_SIZE  4096
 
extern task_struct* s_cur_run_task;
extern uint64_t KB;
extern v_file_node* tarfs_mount_node;
//extern v_file_node* search_file(const char* dir_path, v_file_node * start_node);

uint64_t curr_available_file_des_num = 0;


 dir_info * find_dir(uint64_t des) {
	if (s_cur_run_task == NULL) {
         kprintf("\nKernel Panic : No current running process");
          return NULL;
      }
  
      dir_info * trav = s_cur_run_task->file_root;
      if (trav == NULL) {
          kprintf("\nOpen Files list empty");
          return NULL;
      }
      while (trav != NULL) {
          if (trav->des == des) {
              return trav;
          }
          trav = trav->next;
      }
      return NULL;
 }


dir_info * build_dir_node(uint64_t file_desc, int curr_child_index, v_file_node * v_node) {
      uint64_t addr = KB + kmalloc(sizeof(dir_info));
      dir_info * curr = (struct dir_info *)addr;
      memset((uint8_t *)curr, '\0', PAGE_SIZE);
      curr->des = file_desc;
      curr->curr_child_index = curr_child_index;
	  curr->v_node = v_node;
      curr->next = NULL;
      curr->prev = NULL;
      return curr;
 }



 bool add_dir(int curr_child_index, v_file_node * v_node) {
	if (s_cur_run_task == NULL) {
		kprintf("\nKernel Panic : No current running process");
	    return false;
	}
  
      dir_info * trav = (dir_info *)s_cur_run_task->file_root;
      dir_info * curr = build_dir_node(curr_available_file_des_num++, curr_child_index, v_node);
      curr->next = trav;
      s_cur_run_task->file_root = curr;
      if (trav != NULL) {
          trav->prev = curr;
      }
      return true;
 }

bool delete_dir(uint64_t file_des) {
     dir_info * dir_entry = find_dir(file_des);
     if (dir_entry == NULL) {
          kprintf("Dir entry doesn't exists to be deleted");
          return false;
      }
      dir_info * trav = s_cur_run_task->file_root;
  
      // if first node is to be deleted
      if (trav->des == file_des) {
          dir_info * next = trav->next;
          if (next != NULL) {
              next->prev = NULL;
          }
          s_cur_run_task->file_root = next;
          kfree((uint64_t)dir_entry);
          return true;
      }
  
      while (trav->next->des != dir_entry->des) {
          trav = trav->next;
      }
     trav->next = dir_entry->next;
     if (dir_entry->next != NULL) {
         dir_entry->next->prev = trav;
     }
     kfree((uint64_t)dir_entry);
     return true;
 }



 void print_dir() {
	struct dir_info * root = s_cur_run_task->file_root;
	if (root == NULL) {
		kprintf("\nRoot node is NULL");
	} else {
		dir_info * trav = root;
		kprintf("\nOpen File List is : \n");
		while (trav != NULL) {
			kprintf("%d ", trav->des);
			trav = trav->next;
		}
	}
 }


dir_info * opendir(char *name) {
	v_file_node* search_node = (v_file_node *)search_file(name, tarfs_mount_node);
	if (search_node == NULL) {
		kprintf("\n No folder exists with absolute path - %s", name);
		return NULL;
	}
	if (search_node->is_dir == 0) {
		kprintf("\n File but not folder exists with absolute path - %s", name);
		return NULL;
	}
	int current_child_index = search_node->no_of_child == 0 ? -1 : search_node->no_of_child;
	if (add_dir(current_child_index, search_node)) {
		return find_dir(curr_available_file_des_num - 1);
	}
	return NULL;
}
