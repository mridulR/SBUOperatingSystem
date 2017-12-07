#ifndef _KERN_PROCESS_H
#define _KERN_PROCESS_H

#include <sys/types.h>
#include <sys/vma.h>
#include <sys/terminal.h>
#include <sys/vfs.h>
#include <sys/dirent_s.h>

typedef enum State {
    INIT,
    RUNNING,
    SLEEPING,
    ZOMBIE
}State;

typedef enum Mode {
    KERNEL,
    USER
}Mode;

struct task_struct {
    uint32_t pid;
    uint32_t ppid;
    uint64_t kernel_rsp;
    uint64_t user_rsp;
    uint64_t kstack;
    uint64_t ustack;
    uint32_t exit_status;
    Mode mode;
    uint64_t rip;
    uint64_t pml4; 
    uint64_t entry_addr;
    State state;
    // Used for run_queue
    struct task_struct *next;
    struct task_struct *prev;
    char name[256];
    // For managing VMA
    struct vma* vma_root;
    uint64_t heap_top;
    //Terminal operations
    struct terminal_operation_pntrs term_oprs;
	// FILE Descriptor Table
	struct dir_info *file_root;
    
}__attribute__((packed));

typedef struct task_struct task_struct;

struct process_queue {
    task_struct* cur_task;
    uint64_t nextIndex;
}__attribute__((packed));

typedef struct process_queue Process_queue;

task_struct* create_task(uint64_t parent_pid);

void test_terminal();

#endif
