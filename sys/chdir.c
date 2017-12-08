#include <sys/chdir.h>
#include <sys/types.h>
#include <sys/commons.h>
#include <sys/vfs.h>
#include <sys/kern_process.h>
#include <sys/kprintf.h>
#include <sys/kstring.h>
#include <sys/memset.h>
#include <sys/memcpy.h>
#include <sys/kmalloc.h>


#define PAGE_SIZE  4096
   
  extern task_struct* s_cur_run_task;
  extern uint64_t KB;
  extern v_file_node* tarfs_mount_node;

  // We support only absolute path i.e path from rootfs/......
  int chdir(const char *path) {
	if (s_cur_run_task == NULL) {
         kprintf("\nKernel Panic : No current running process");
         return -1;
     }

	if (path == NULL || kstrlen(path) == 0) {
		kprintf("\n Setting current working directory to home - rootfs/");
		memset((void *)s_cur_run_task->cwd, '\0', kstrlen((char *)s_cur_run_task->cwd));
		memcpy((void *)s_cur_run_task->cwd,(char *)"rootfs/" ,kstrlen((char *)s_cur_run_task->cwd));
		return 0;
	}
	
	if (kstrlen(path) == 1 && path[0] == '.') {
		// Do nothing
		return 0;
	}

    if (kstrlen(path) == 2 && path[0] == '.' && path[1] == '.') {
		if (kstrcmp((char *)s_cur_run_task->cwd, "rootfs/") == 0) {
			return 0;
		}
	    char rst_name[256];
		memset((uint8_t *)rst_name, '\0', 256);

		char name[256];
        memset((uint8_t *)name, '\0', 256);
		memcpy(name, (char *)s_cur_run_task->cwd, kstrlen(s_cur_run_task->cwd));
		char *remaining = name;
		char *temp = kstr_tok(name, '/', &remaining);
		k_concat(rst_name, temp);
		k_concat(rst_name, "/");

        while (remaining != NULL && kstrlen(remaining) != 0) {
			temp = kstr_tok(remaining, '/' , &remaining);
			if (remaining != NULL && kstrlen(remaining) != 0) {
				k_concat(rst_name, temp);
				k_concat(rst_name, "/");
			}
        }
		memset((void *)s_cur_run_task->cwd, '\0', kstrlen((char *)s_cur_run_task->cwd));
		memcpy((void *)s_cur_run_task->cwd, rst_name ,kstrlen(rst_name));
		return 0;
	}

	v_file_node * search_node  = search_file(path, tarfs_mount_node);
	if (search_node == NULL || search_node->is_dir == 0) {
		kprintf("\n No such directory exists - %s", path);
		return -1;
	}

	memset((void *)s_cur_run_task->cwd, '\0', kstrlen((char *)s_cur_run_task->cwd));
	memcpy((void *)s_cur_run_task->cwd, path ,kstrlen(path));

    return 0;
  }


  char *sys_getcwd(char *buf, int size) {
	if (s_cur_run_task == NULL) {
		kprintf("\nKernel Panic : No current running process");
		return NULL;
	}
	memset((void *)buf, '\0', size);
	memcpy(buf, (void *)s_cur_run_task->cwd, kstrlen((char *)s_cur_run_task->cwd));
	return buf;
  }
