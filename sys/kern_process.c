#include <sys/kern_process.h>
#include <sys/kmalloc.h>
#include <sys/kprintf.h>
#include <sys/memset.h>
#include <sys/memcpy.h>
#include <sys/gdt.h>
#include <sys/idt.h>

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
    task->kernel_rsp = 0;
    task->user_rsp = 0;
    task->kstack = kmalloc(PAGE_SIZE);
    memset(task->kstack, 0, PAGE_SIZE);
    task->ustack = kmalloc(PAGE_SIZE);
    memset(task->ustack, 0, PAGE_SIZE);
    task->exit_status = 0;
    task->state = RUNNING;
    memcpy(&(task->name),"Process", 7);
    return task;
}

void switch_to(task_struct *cur, task_struct *next);

void first_switch_to(task_struct *cur, task_struct *next);

void test_user_function()
{
    kprintf(" Did I Crash?");
    
    __asm__ __volatile__
    (
      "movq $0x20, %%rsi\n"
      "movq $0x30, %%rdi\n"
      :
      :
    );

    __asm__ __volatile__("int $0x80\n"); 

    while(1) {}
    /*__asm__ __volatile__ 
    (
        "cli;\n" 
        :
        :
    );
    int a = 10;
    int b = 20;
    int c;
    c = a + b;
    b = c;
    __asm__ __volatile__ ("iretq\n");*/
}

void switch_to_ring3() {

    set_tss_rsp((uint64_t *)s_task_1->kernel_rsp);

    __asm__ __volatile__
    (  "pushq $0x23\n"
       "pushq %0\n"
       "pushfq\n"
       "pushq $0x2b\n"
       :
       :"r" (s_task_2->user_rsp)
    );
    
    __asm__ __volatile__ 
    (   "pushq %0\n"
        "iretq\n"
        :
        :"r" (&test_user_function)
    );
}

void function_2(int d) {
    int a = 20;
    kprintf("\nIn Process 2");
    switch_to(s_task_2, s_task_1);
    int b = 60;
    kprintf("\nResuming Process 2.1");
    switch_to(s_task_2, s_task_1);
    int c = a + b;
    kprintf("\nResuming Process 2.2");
    kprintf(" c = %d", c);
    switch_to(s_task_2, s_task_1);
    kprintf("\nResuming Process 2.3");
    kprintf(" c = %d", c);
    uint8_t *ptr = (uint8_t *)0xFFFFFFFF802000B0;
    kprintf(" Value = %d", *ptr);
    switch_to_ring3();
    //test_user_function();
    while(1) { }
    return;
}

void function_1() {
    kprintf("\nIn Process 1");
    first_switch_to(s_task_1, s_task_2);
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

