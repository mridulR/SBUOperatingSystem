#include <sys/kern_process.h>
#include <sys/kmalloc.h>
#include <sys/kprintf.h>
#include <sys/memset.h>
#include <sys/memcpy.h>
#include <sys/vfs.h>
#include <sys/elf64.h>
#include <sys/kstring.h>
#include <sys/tarfs.h>
#include <sys/gdt.h>
#include <sys/idt.h>
#include <sys/types.h>
#include <sys/phys_pages.h>
#include <sys/page_table.h>
#include <sys/vma.h>
#include <sys/terminal.h>

#define PAGE_SIZE 0x1000

extern uint64_t KB;
extern uint64_t PS;
extern int64_t terminal_read(int fd, void * buf, uint64_t count);
extern int64_t terminal_write(int fd, void * buf, uint64_t count);

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
    uint64_t addr = KB + kmalloc(PAGE_SIZE);
    task_struct *task = (task_struct *)addr;
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
    task->kstack = kmalloc(PAGE_SIZE);
    memset((uint64_t *)(KB + task->kstack), 0, PAGE_SIZE);
    //task->ustack = kmalloc(PAGE_SIZE);
    task->ustack = 0;
    //memset((uint64_t *)(KB + task->ustack), 0, PAGE_SIZE);
    task->kernel_rsp = KB + task->kstack + PS;
    //task->user_rsp   = KB + task->ustack + PS;
    task->user_rsp   = 0;
    task->exit_status = 0;
    task->state = RUNNING;
    task->mode = KERNEL;
    task->rip = 0;
    task->pml4 = kmalloc(PAGE_SIZE);
    //kprintf(" USER PML4: %p ", task->pml4);
    memcpy((uint64_t *)(KB + task->pml4), (uint64_t *)(KB + s_pml4_table), PAGE_SIZE);
    task->next = NULL;
    task->prev = NULL;
    memset(&(task->name),'\0', 256);
    if(s_cur_free_process_index == 0) {
        memcpy(&(task->name),"SBUSH", 5);
    }
    else {
        memcpy(&(task->name),"Process", 7);
    }
    //vma entries
    task->vma_root = NULL;
    task->heap_top = 0;   // Should be filled from elf file
    // Terminal Operations
    task->term_oprs.terminal_read  = &terminal_read;         
    task->term_oprs.terminal_write = &terminal_write;         

    return task;
}

void freeTask(task_struct *task) {
    kfree(task->kstack);
    kfree(task->ustack);
    kfree((uint64_t)task);
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
    kprintf("\nResuming Process 2.3\n");

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

void test_vma_print_empty_vma_list() {
   print_vma();
}

void test_vma_find_node_from_empty_vma_list() {
    vma *node = find_vma(30);
     if (node == NULL) {
        kprintf("\nPASS : Node search from empty list is NULL");
     } else {
        kprintf("\nFAIL : Node search from empty list is not NULL");
     }
}

void test_vma_add_node_to_empty_vma_list() {
    if(add_vma(12345)) {
        vma * node = find_vma(0); // This is heap top now
        if (node == NULL) {
            kprintf("\nFAIL : searching node after adding to empty vma list failed");
        } else {
            if (node->start_addr == 0 && node->end_addr == 12345) {
                if(delete_vma(0)) {
                    node = find_vma(0);
                    if (node == NULL) {
                        kprintf("\nPASS : EmptyList -> Add -> find -> delete -> find");
                    } else {
                        kprintf("\nFAIL : Deleting node after adding to empty vma list unsuccessful");
                    }
                } else {
                    kprintf("\nFAIL : Deleting node after adding to empty vma list failed");
                }                
            } else {
                kprintf("\nFAIL : searching node after adding to empty vma list returned wrong node");
            }
        }
    } else {
        kprintf("\nFAIL : Adding node to empty VMA list failed");
    }
}

void test_vma_operations_on_bigger_list() {
    s_cur_run_task->heap_top = 0; // Initializing vma again
    add_vma(1); // start = 0, end = 1
    add_vma(2); // start = 1, end = 3
    add_vma(3); // start = 3, end = 6
    add_vma(4); // start = 6, end = 10
    add_vma(5); // start = 10, end = 15
    add_vma(5); // start = 15, end = 20
    add_vma(4); // start = 20, end = 24
    add_vma(3); // start = 24, end = 27
    
    print_vma();
    vma * node = find_vma(15);
    if (node != NULL && node->start_addr == 15 && node->end_addr == 20) {
        kprintf("\nPASS : Search middle element from list");
    } else {
        kprintf("\nFAIL : Search middle element from list");
    }

    node = find_vma(0);
    if (node != NULL && node->start_addr == 0 && node->end_addr == 1) {
        kprintf("\nPASS : Search last element from list");
    } else {
        kprintf("\nFAIL : Search last element from list");
    }

    node = find_vma(24);
    if (node != NULL && node->start_addr == 24 && node->end_addr == 27) {
        kprintf("\nPASS : Search first element from list");
    } else {
        kprintf("\nFAIL : Search first element from list");
    }

    delete_vma(15);
    node = find_vma(15);
    if (node != NULL && node->start_addr == 15 && node->end_addr == 20) {
        kprintf("\nFAIL : Search middle element from list after delete");
    } else {
        kprintf("\nPASS : Search middle element from list after delete");
    }  

    delete_vma(0);
    node = find_vma(0);
    if (node != NULL && node->start_addr == 0 && node->end_addr == 1) {
        kprintf("\nFAIL : Search last element from list after delete");
    } else {
        kprintf("\nPASS : Search last element from list after delete");
    }

    delete_vma(24);
    node = find_vma(24);
    if (node != NULL && node->start_addr == 24 && node->end_addr == 27) {
        kprintf("\nFAIL : Search first element from list after delete");
    } else {
        kprintf("\nPASS : Search first element from list after delete");
    }

    print_vma();
}

void test_vma_operations() {
     s_cur_run_task = s_init_process;
     test_vma_print_empty_vma_list();
     test_vma_find_node_from_empty_vma_list();
     test_vma_add_node_to_empty_vma_list();
     test_vma_operations_on_bigger_list();
}

void test_terminal() {
    kprintf("\n Testing terminal \n");
    uint64_t addr = KB + kmalloc(PAGE_SIZE);
    char *buf = (char *)addr;

    uint64_t count = 0;
    while(count != 4099) {
        terminal_enqueue('b');
        count++;
    }
	terminal_enqueue('\n');

    while(1) {
    int len = terminal_read(0, buf, PAGE_SIZE);
    kprintf("\n Writing buff received from terminal =====> %s\n", buf);
    kprintf("\n Writing to terminal =======> ");
    terminal_write(1, buf, len);
    kprintf("\n");
    }
     
    kfree((uint64_t) addr);
}


void LaunchSbush(){
    kprintf("\nLaunching Sbush...");
    s_sbush_process = create_elf_process("rootfs/bin/sbush", NULL);
    kprintf("\n SBUSH:%d, (P:%d, PP:%d) %p", 0, s_sbush_process->pid, s_sbush_process->ppid, s_sbush_process);
    if (s_sbush_process == NULL) {
        kprintf("\nSBUSH launch not implemented.....\n");
    }
    kprintf(" Test: in kern_process.c");
    //test_vma_operations();

    //kprintf("\n SBUSH:%d, (P:%d, PP:%d) %p", 0, s_sbush_process->pid, s_sbush_process->ppid, s_sbush_process);
    //switch_to_ring3();
    //test_user_function();
    while(1) {}
    return;
}

void init_start() {

    kprintf("\nInit Process Launched\n");
    init_process_queue();
    init_tarfs();
    print_node_inorder(root_node);

    //test_terminal();
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
