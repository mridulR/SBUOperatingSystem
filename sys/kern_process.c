#include <sys/kern_process.h>
#include <sys/kmalloc.h>
#include <sys/kprintf.h>
#include <sys/memset.h>
#include <sys/memcpy.h>
#include <sys/vfs.h>
#include <sys/kstring.h>
#include <sys/tarfs.h>

#define PAGE_SIZE 0x1000

uint8_t flag = 0;

task_struct * s_task_1, *s_task_2;
uint32_t s_cur_pid = 1;

task_struct* create_task() {
    task_struct *task = (task_struct *)kmalloc(PAGE_SIZE);
    memset(task, 0, PAGE_SIZE);
    if(!task) {
        kprintf("KERNEL PANIC: Can't allocate free page for task_struct !!!");
        return NULL;
    }
    task->pid = s_cur_pid++;
    task->pid = 1;
    task->rsp = 0;
    task->kstack = kmalloc(PAGE_SIZE);
    memset(task->kstack, 0, PAGE_SIZE);
    task->exit_status = 0;
    task->state = RUNNING;
    memcpy(&(task->name),"Process", 7);
    return task;
}


void switch_to(task_struct *cur, task_struct *next) {

    __asm__ __volatile__         
    (                            
       "pushq %%rax\n"
       "pushq %%rbx\n"
       "pushq %%rcx\n"
       "pushq %%rdx\n"
       "pushq %%rbp\n"
       "pushq %%rdi\n"
       "pushq %%rsi\n"
       "pushq %%r8\n"
       "pushq %%r9\n"
       "pushq %%r10\n"
       "pushq %%r11\n"
       "pushq %%r12\n"
       "pushq %%r13\n"
       "pushq %%r14\n"
       "pushq %%r15\n"
       "movq %%rsp, %0\n"
       :"=r" (cur->rsp)
       :
     );

     __asm__ __volatile__
     (
       "movq %0, %%rsp\n"
       :
       :"r" (next->rsp)    
      );

      __asm__ __volatile__
      (
       "popq %%r15\n"
       "popq %%r14\n"
       "popq %%r13\n"
       "popq %%r12\n"
       "popq %%r11\n"
       "popq %%r10\n"
       "popq %%r9\n"
       "popq %%r8\n"
       "popq %%rsi\n"
       "popq %%rdi\n"
       "popq %%rbp\n"
       "popq %%rdx\n"
       "popq %%rcx\n"
       "popq %%rbx\n"
       "popq %%rax\n"
       "retq"
       :
       : 
     );
     return;
}

void function_2() {

	kprintf("\nTesting tarfs \n");
	v_file_node * root_node = init_tarfs();
	print_node_inorder(root_node);

	kprintf("\n Lookup sbush file \n");
	v_file_node * result = search_file("rootfs/bin/sbush", root_node->v_child[0]);
	
	if (result != NULL) {
		kprintf("Sbush found - %s, start addr = %p, end_addr = %p\n", 
			result->v_name, result->start_addr, result->end_addr);
	} else {
		kprintf("Sbush not found !!!!\n");
	}
    
    kprintf("\nIn Process 2");
    switch_to(s_task_2, s_task_1);
    kprintf("\nResuming Process 2.1");
    switch_to(s_task_2, s_task_1);
    kprintf("\nResuming Process 2.2");
    switch_to(s_task_2, s_task_1);
    kprintf("\nResuming Process 2.3\n");

    while(1) { }
    return;
}


void first_switch(task_struct *cur, task_struct *next) {
    __asm__ __volatile__
    ( 
       "pushq %%rax\n"
       "pushq %%rbx\n"
       "pushq %%rcx\n"
       "pushq %%rdx\n"
       "pushq %%rbp\n"
       "pushq %%rdi\n"
       "pushq %%rsi\n"
       "pushq %%r8\n"
       "pushq %%r9\n"
       "pushq %%r10\n"
       "pushq %%r11\n"
       "pushq %%r12\n"
       "pushq %%r13\n"
       "pushq %%r14\n"
       "pushq %%r15\n"
       "movq %%rsp, %0\n"
       :"=r" (cur->rsp)
       :
    );
    __asm__ __volatile__
    (
       "movq %0, %%rsp\n"
       :
       :"r" (next->rsp)
    );

    __asm__ __volatile__
    ( "pushq %0\n"
      "retq"
      : 
      :"r" (&function_2)
    );
    return;
}

void function_1() {
    kprintf("\nIn Process 1");
    first_switch(s_task_1, s_task_2);
    kprintf("\nIn Process 1.1");
    switch_to(s_task_1, s_task_2);
    kprintf("\nResuming Process 1.2");
    switch_to(s_task_1, s_task_2);
    kprintf("\nResuming Process 1.3");
    switch_to(s_task_1, s_task_2);
    kprintf("\nResuming Process 1.4");
    while(1) { }
    return;
}

