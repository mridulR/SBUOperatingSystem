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
    WAITING,
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
    uint32_t child_exit_status;
    uint32_t child_exit_pid;
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
    uint64_t heap_start;
    //Terminal operations
    struct terminal_operation_pntrs term_oprs;
	// FILE Descriptor Table
	struct dir_info *file_root;
	// Current Workind Directory
	char cwd[256];
    
}__attribute__((packed));

typedef struct task_struct task_struct;

struct process_queue {
    task_struct* cur_task;
    uint64_t nextIndex;
}__attribute__((packed));

typedef struct process_queue Process_queue;

task_struct* create_task(uint64_t parent_pid);
void printRunQueue();
void init_process_queue();
void add_new_task_to_run_queue_start(task_struct *task);
void add_new_task_to_run_queue_end(task_struct *task);
void remove_task_from_process_queue(uint64_t pid);
void remove_task_from_run_queue(task_struct *task);
task_struct* copy_task_struct(task_struct *copy_task);
uint64_t sys_fork();
void freeTask(task_struct *task);
void kill_task(uint64_t pid);
void LaunchSbush();
void init_start();
uint64_t sys_yield();
task_struct* find_process_with_pid(int pid);
task_struct* find_first_child(int pid);
int sys_wait(uint64_t status);
int sys_waitpid(int pid, uint64_t status, int options);
void sys_kill(int flag, int pid);
void sys_exit(int status);
void sys_ps();
void sys_sleep(int time);
void schedule_next(task_struct* cur, task_struct* next);
uint64_t sys_fork();

#endif
