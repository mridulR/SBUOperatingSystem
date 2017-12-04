#include <sys/kern_process.h>
#include <sys/kmalloc.h>
#include <sys/kprintf.h>
#include <sys/memset.h>
#include <sys/memcpy.h>
#include <sys/gdt.h>
#include <sys/idt.h>
#include <sys/types.h>
#include <sys/phys_pages.h>
#include <sys/page_table.h>

#define PAGE_SIZE 0x1000


Process_queue s_process_queue[2048];

uint64_t s_free_process_count      = 2048;
uint64_t s_max_process_count       = 2048;
uint64_t s_cur_free_process_index  = 0;
extern uint64_t s_pml4_table;

task_struct* s_run_queue_head = NULL;
task_struct* s_run_queue_tail = NULL;
task_struct* s_cur_run_task   = NULL;
task_struct* s_last_task      = NULL;
task_struct* s_init_process   = NULL;
task_struct* s_sbush_process  = NULL;

uint64_t s_cur_kernel_rsp;
uint64_t s_cur_user_rsp;
uint64_t s_last_kernel_rsp;
uint64_t s_last_user_rsp;

void init_process_queue() {
    for(int count = 0; count < s_max_process_count; ++count){
        s_process_queue[count].cur_task  = NULL;
        s_process_queue[count].nextIndex = count + 1;
    }
}

void add_new_task_to_run_queue(task_struct *task) {
    if(s_run_queue_head == NULL) {
        s_run_queue_head = task;
        s_run_queue_tail = task;
        return;
    }
    task->next = s_run_queue_head;
    task->prev = NULL;
    s_run_queue_head = task;
    return;
}


void remove_task_from_process_queue(uint64_t pid) {
    uint64_t save = s_cur_free_process_index;
    s_cur_free_process_index =  pid;
    s_process_queue[pid].nextIndex = save;
    return;
}

void remove_task_from_run_queue(task_struct *task) {
   //Remove from head
   if(task == s_run_queue_head) {
      if(s_run_queue_head->next == NULL) {
          s_run_queue_head = s_run_queue_head->next;
          s_run_queue_tail = s_run_queue_head;
          return;
      }
      s_run_queue_head->next->prev = NULL;
      s_run_queue_head = s_run_queue_head->next;
   }

   //Remove from tail 
   if(task == s_run_queue_tail) {
       task_struct *save;
       save = s_run_queue_tail->prev;
       save->next = NULL;
       s_run_queue_tail->prev = NULL;
       s_run_queue_tail->next = NULL;
       s_run_queue_tail = save;
       return;
   }

   //Remove from middle
   task_struct* trav = s_run_queue_head;
   task_struct* save = NULL;
   while(trav->next != NULL){
       save = trav->prev;
       if(trav == task){
         save->next = trav->next;
         trav->next->prev = save;
         trav->next = NULL;
         trav->prev = NULL;
       }
       trav = trav->next;
   }
   return;
}

task_struct* create_task(uint64_t ppid) {
    if(s_free_process_count == 0){
        kprintf("KERNEL PANIC: CREATE TASK(): Max Process limit reached !!!");
        return NULL;
    }
    if(ppid > s_max_process_count){
        kprintf("KERNEL PANIC: Invalid Parent process Id !!!");
        return NULL;
    }

    task_struct *task = (task_struct *)kmalloc(PAGE_SIZE);
    memset(task, 0, PAGE_SIZE);
    if(!task) {
        kprintf("KERNEL PANIC: CREATE TASK(): Can't allocate free page for task_struct !!!");
        return NULL;
    }
    if(s_cur_free_process_index == 0) {
        task->ppid = -1;
        s_sbush_process = task;
        s_process_queue[0].cur_task  = s_sbush_process;
        task->pid = s_cur_free_process_index;
        s_cur_free_process_index = s_process_queue[0].nextIndex;
        s_run_queue_head = s_sbush_process;
        --s_free_process_count;
    }
    else {
        s_process_queue[s_cur_free_process_index].cur_task = task;
        task->pid = s_cur_free_process_index;
        task->ppid = ppid;
        s_cur_free_process_index = s_process_queue[s_cur_free_process_index].nextIndex;
        --s_free_process_count;
    }
    task->kernel_rsp = 0;
    task->user_rsp = 0;
    task->kstack = kmalloc(PAGE_SIZE);
    memset(task->kstack, 0, PAGE_SIZE);
    task->ustack = kmalloc(PAGE_SIZE);
    memset(task->ustack, 0, PAGE_SIZE);
    task->exit_status = 0;
    task->state = RUNNING;
    task->mode = KERNEL;
    task->rip = 0;
    task->pml4 = allocate_phys_page();
    //task->pml4 = (uint8_t *)allocate_phys_page();
    kprintf(" USER PML4: %p ", task->pml4);
    //memcpy((uint8_t *)(0xFFFFFFFF80000000 + task->pml4), (0xFFFFFFFF80000000 + s_pml4_table), PAGE_SIZE);
    task->next = NULL;
    task->prev = NULL;
    memset(&(task->name),'\0', 256);
    if(s_cur_free_process_index == 0) {
        memcpy(&(task->name),"SBUSH", 5);
        return task;
    }
    else {
        memcpy(&(task->name),"Process", 7);
        return task;
    }
    return task;
}

void freeTask(task_struct *task) {
    kfree(task->kstack);
    kfree(task->ustack);
    kfree(task);
    return;
}

void kill_task(uint64_t pid) {
    if(pid <= 0 || pid > s_max_process_count || s_process_queue[pid].cur_task == NULL) {
        kprintf("KERNEL PANIC: Invalid Process id !!! (%d) ", pid);
        return ;
    }
    task_struct* pcb;
    pcb = s_process_queue[pid].cur_task; 
    remove_task_from_run_queue(pcb);
    remove_task_from_process_queue(pid);
    freeTask(pcb);
    return;
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

    set_tss_rsp((uint64_t *)s_init_process->kernel_rsp);

    __asm__ __volatile__
    (  "pushq $0x23\n"
       "pushq %0\n"
       "pushfq\n"
       "pushq $0x2b\n"
       :
       :"r" (s_sbush_process->user_rsp)
    );
    
    __asm__ __volatile__ 
    (   "pushq %0\n"
        "iretq\n"
        :
        :"r" (&test_user_function)
    );
}

/*void function_2(int d) {
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
}*/

void schedule() {

}

void LaunchSbush(){
    kprintf("\nLaunching Sbush...");
    s_sbush_process = create_task(0);
    kprintf("\n SBUSH:%d, (P:%d, PP:%d) %p", 0, s_sbush_process->pid, s_sbush_process->ppid, s_sbush_process);
   // __asm__ __volatile__("cli\n");
    //set_cr3_register((PML4 *)s_init_process->pml4);
    //__asm__ __volatile__("sti\n");
    //uint64_t *ptr = (uint64_t *)0x300000;
    //*ptr = 99; 
    //kprintf(" %d " , *ptr); 
    //switch_to_ring3();
    //test_user_function();
    return;
}

void init_start() {

    kprintf("\nInit Process Launched");
    init_process_queue();
    LaunchSbush();
    while(1) {
        schedule();
    }

    /*first_switch_to(s_task_1, s_task_2);
    kprintf("\nIn Process 1.1");
    switch_to(s_task_1, s_task_2);
    kprintf("\nResuming Process 1.2");
    switch_to(s_task_1, s_task_2);
    kprintf("\nResuming Process 1.3");
    switch_to(s_task_1, s_task_2);
    kprintf("\nResuming Process 1.4");*/
    while(1) { }
    return;
}


void test_process_queue() {
    task_struct *task;
    task = create_task(0);
    //kprintf("\n Task%d, (P:%d, PP:%d) %p", 1, task->pid, task->ppid, task);
    task = create_task(0);
    //kprintf("\n Task%d, (P:%d, PP:%d) %p", 2, task->pid, task->ppid, task);
    task = create_task(0);
    //kprintf("\n Task%d, (P:%d, PP:%d) %p", 3, task->pid, task->ppid, task);
    task = create_task(0);
    //kprintf("\n Task%d, (P:%d, PP:%d) %p", 4, task->pid, task->ppid, task);
    task = create_task(0);
    //kprintf("\n Task%d, (P:%d, PP:%d) %p", 5, task->pid, task->ppid, task);
    task = create_task(0);
    //kprintf("\n Task%d, (P:%d, PP:%d) %p", 6, task->pid, task->ppid, task);
    kprintf("\n Task%d, (P:%d, PP:%d) %p", 7, task->pid, task->ppid, task);
    for(int i = 0; i<10; ++i) {
        if(s_process_queue[i].cur_task == NULL) {
            kprintf("\nRk PQ[%d], (NI: %d) %p", i,
                    s_process_queue[i].nextIndex, s_process_queue[i].cur_task);
        }
        else {
            kprintf("\nRk PQ[%d], (P:%d, NI: %d, PP:%d) %p", i,
                    s_process_queue[i].cur_task->pid, s_process_queue[i].nextIndex,
                    s_process_queue[i].cur_task->ppid, s_process_queue[i].cur_task);
        }
    }
    kill_task(0);
    kill_task(1);
    kill_task(2);
    kill_task(3);
    kill_task(4);
    kill_task(5);
    kill_task(6);
    task = create_task(0);
    task = create_task(0);
    task = create_task(0);
    task = create_task(0);
    task = create_task(0);
    task = create_task(0);
    kill_task(0);
    kill_task(1);
    kill_task(2);
    kill_task(3);
    kill_task(4);
    kill_task(5);
    kill_task(6);
    task = create_task(0);
    task = create_task(0);
    task = create_task(0);
    task = create_task(0);
    task = create_task(0);
    task = create_task(0);
    kill_task(0);
    kill_task(1);
    kill_task(2);
    kill_task(3);
    kill_task(4);
    kill_task(5);
    kill_task(6);
 
    kprintf("\n Task%d, (P:%d, PP:%d) %p", 7, task->pid, task->ppid, task);
    for(int i = 0; i<10; ++i) {
        if(s_process_queue[i].cur_task == NULL) {
            kprintf("\n PQ[%d], (NI: %d) %p", i,
                    s_process_queue[i].nextIndex, s_process_queue[i].cur_task);
        }
        else {
            kprintf("\n PQ[%d], (P:%d, NI: %d, PP:%d) %p", i,
                    s_process_queue[i].cur_task->pid, s_process_queue[i].nextIndex,
                    s_process_queue[i].cur_task->ppid, s_process_queue[i].cur_task);
        }
    }
}
