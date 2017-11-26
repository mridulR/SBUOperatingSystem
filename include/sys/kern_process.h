#ifndef _KERN_PROCESS_H
#define _KERN_PROCESS_H

#include <sys/types.h>

typedef enum State {
    INIT,
    RUNNING,
    SLEEPING,
    ZOMBIE
}State;

struct task_struct {
    uint32_t pid;
    uint32_t ppid;
    uint64_t kernel_rsp;
    uint64_t user_rsp;
    uint8_t* kstack;
    uint8_t* ustack;
    uint32_t exit_status;
    State state;
    char name[256];
}__attribute__((packed));

typedef struct task_struct task_struct;

task_struct* create_task();

#endif
