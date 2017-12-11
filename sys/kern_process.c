#include <sys/commons.h>
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
#include <sys/dirent_s.h>
#include <sys/chdir.h>
#include <sys/pit.h>

#define PAGE_SIZE 0x1000

//extern uint32_t g_pit_count;
extern struct reg_info* reg;
extern uint64_t KB;
extern uint64_t PS;
extern uint64_t UB;
extern int64_t terminal_read(int fd, void * buf, uint64_t count);
extern int64_t terminal_write(int fd, void * buf, uint64_t count);

extern struct dir_info * find_dir(uint64_t des);
extern bool add_dir(int curr_child_index, struct v_file_node * v_node);
extern bool delete_dir(uint64_t des);
extern void print_dir();
extern struct dir_info * sys_opendir(char *name);
extern struct dirent *sys_readdir(struct dir_info *dirp);
extern int sys_closedir(struct dir_info *dirp);
extern int sys_open(const char *pathname, int flags);
extern int sys_close(int fd);
extern int sys_read(int fd, void *buf, int count);

extern v_file_node* root_node;
extern v_file_node* tarfs_mount_node;
extern v_file_node* get_root_node();
extern v_file_node* search_file(const char* dir_path, v_file_node * start_node);
extern void print_node_inorder(v_file_node* root);

extern int sys_chdir(const char *path);
extern char *sys_getcwd(char *buf, int size);

Process_queue s_process_queue[2048];

uint64_t s_free_process_count      = 2048;
uint64_t s_max_process_count       = 2048;
uint64_t s_cur_free_process_index  = 1;
extern uint64_t s_pml4_table;

task_struct* s_run_queue_head = NULL;
task_struct* s_run_queue_tail = NULL;
task_struct* s_cur_run_task   = NULL;
task_struct* s_last_task      = NULL;
task_struct* s_init_process   = NULL;
task_struct* s_sbush_process  = NULL;

uint64_t s_run_queue_count = 0;
uint64_t s_cur_kernel_rsp;
uint64_t s_cur_user_rsp;
uint64_t s_last_kernel_rsp;
uint64_t s_last_user_rsp;

void printRunQueue() {
    task_struct* proc = s_run_queue_head;
    while(proc != NULL){
        kprintf(" Proceess %d \n ", proc->pid);
        proc = proc->next;
    }
}

void print_process_queue(){
    for(int i =0; i<10; ++i) {
        kprintf(" (%d, %d)  ", i, s_process_queue[i].nextIndex);
    }
    return;
}

void init_process_queue() {
    for(int count = 0; count < s_max_process_count; ++count){
        s_process_queue[count].cur_task  = NULL;
        s_process_queue[count].nextIndex = count + 1;
    }
}

void add_new_task_to_run_queue_start(task_struct *task) {
    if(s_run_queue_head == NULL) {
        s_run_queue_head = task;
        s_run_queue_tail = task;
        ++s_run_queue_count;
        return;
    }
    task->next = s_run_queue_head;
    task->prev = NULL;
    s_run_queue_head = task;
    ++s_run_queue_count;
    return;
}

void add_new_task_to_run_queue_end(task_struct *task) {
    if(s_run_queue_head == NULL) {
        s_run_queue_head = task;
        s_run_queue_tail = task;
        ++s_run_queue_count;
        return;
    }
    task->next = NULL;
    task->prev = s_run_queue_tail;
    task->prev->next = task;
    s_run_queue_tail = task;
    ++s_run_queue_count;
    return;
}

void remove_task_from_process_queue(uint64_t pid) {
    uint64_t save = s_cur_free_process_index;
    s_cur_free_process_index =  pid;
    s_process_queue[pid].nextIndex = save;
    return;
}

// Test this once
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
      --s_run_queue_count;
      return;
   }

   //Remove from tail
   if(task == s_run_queue_tail) {
       task_struct *save;
       save = s_run_queue_tail->prev;
       save->next = NULL;
       s_run_queue_tail->prev = NULL;
       s_run_queue_tail->next = NULL;
       s_run_queue_tail = save;
       --s_run_queue_count;
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
   --s_run_queue_count;
   return;
}

task_struct* copy_task_struct(task_struct *copy_task) {

    if(s_free_process_count == 0){
        kprintf("KERNEL PANIC: FORK FAILED: Max Process limit reached !!!");
        return NULL;
    }
    if(copy_task->pid > s_max_process_count){
        kprintf("KERNEL PANIC: Invalid Parent process Id !!!");
        return NULL;
    }
    uint64_t addr = KB + kmalloc(PAGE_SIZE);
    task_struct *task = (task_struct *)addr;
    memset(task, 0, PAGE_SIZE);
    if(!task) {
        kprintf("KERNEL PANIC: FORK FAILED: Can't allocate free page for task_struct !!!");
        return NULL;
    }
    s_process_queue[s_cur_free_process_index].cur_task = task;
    task->pid = s_cur_free_process_index;
    task->ppid = copy_task->pid;
    s_cur_free_process_index = s_process_queue[s_cur_free_process_index].nextIndex;
    --s_free_process_count;

    // copy kstack
    task->kstack = kmalloc(PAGE_SIZE);
    memset((uint64_t *)(KB + task->kstack), 0, PAGE_SIZE);
    memcpy((uint64_t *)(KB + task->kstack), (uint64_t *)(KB + copy_task->kstack), PAGE_SIZE);

    // copy ustack
    task->ustack = kmalloc(PAGE_SIZE);
    memset((uint64_t *)(KB + task->ustack), 0, PAGE_SIZE);
    memcpy((uint64_t *)(KB + task->ustack), (uint64_t *)(UB), PAGE_SIZE);

    // update the rsp
    task->kernel_rsp = KB + task->kstack + PS;
    task->user_rsp   = UB + PS;

    task->child_exit_status = -1;
    task->child_exit_pid = -1;
    task->state = INIT;
    task->mode = KERNEL;
    task->rip = 0;

    // copy pml4
    task->pml4 = kmalloc(PAGE_SIZE);
    //kprintf(" USER PML4: %p ", task->pml4);
    memset((uint64_t *)(KB + task->pml4), 0, PAGE_SIZE);
    memcpy((uint64_t *)(KB + task->pml4), (uint64_t *)(KB + copy_task->pml4), PAGE_SIZE);
    task->next = NULL;
    task->prev = NULL;
    memset(&(task->name),'\0', 256);
    if(task->pid  == 0) {
        memcpy(&(task->name),"SBUSH", 5);
    }
    else {
        memcpy(&(task->name),"Process", 7);
    }
    //vma entries
    task->vma_root = copy_vma_list(copy_task->vma_root);
    task->heap_top   = copy_task->heap_top;
    task->heap_start = copy_task->heap_start;

    // Terminal Operations
    task->term_oprs.terminal_read  = &terminal_read;
    task->term_oprs.terminal_write = &terminal_write;

	// File Descriptor Table  ----  Since terminal are mapped separately set it to NUll initially
    task->file_root = NULL;

	// Current working directory should be first set to 'rootfs/bin'
	memcpy(&(task->cwd),"rootfs/bin", 10);
    return task;
}

void update_run_queue() {
  if(s_run_queue_head != s_run_queue_tail) {
      task_struct * save = s_run_queue_head->next;
      s_run_queue_tail->next = s_run_queue_head;
      s_run_queue_head->next = NULL;
      s_run_queue_head->prev = s_run_queue_tail;
      s_run_queue_tail = s_run_queue_head;
      s_run_queue_head = save;
  }
  return;
}


void switch_to(task_struct *cur, task_struct *next) {
    cur->kernel_rsp  = (uint64_t) reg;
    next->kernel_rsp = next->kernel_rsp - sizeof(struct reg_info);
    uint64_t diff = cur->user_rsp - reg->rsp;
    reg->rsp = next->user_rsp - diff;
    set_tss_rsp((uint64_t *)next->kernel_rsp);
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
        s_run_queue_tail = s_sbush_process;
        --s_free_process_count;
        ++s_run_queue_count;
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
    task->state = INIT;
    task->mode = KERNEL;
    task->child_exit_status = -1;
    task->child_exit_pid    = -1;
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
	// File Descriptor Table  ----  Since terminal are mapped separately set it to NUll initially
    task->file_root = NULL;
	// Current working directory should be first set to 'rootfs/bin'
	memcpy(&(task->cwd),"rootfs/bin", 10);
    return task;
}

uint64_t sys_fork() {
  task_struct* task = copy_task_struct(s_cur_run_task);
  add_new_task_to_run_queue_end(task);
  sys_yield();
  return task->pid;
}

void schedule_next(task_struct* cur, task_struct* next) {
    if(next != NULL && cur != next) {
        kprintf(" Will do Context Switch cur(%d) to next(%d) !!!", s_cur_run_task->pid, next->pid);
        set_cr3_register(next->pml4);
        __asm__ __volatile__ ("movq %%cr3,%%rax\n" : : );
        __asm__ __volatile__ ("movq %%rax,%%cr3\n" : : );
        switch_to(cur, next);
        s_cur_run_task = next;
        next->state = RUNNING;
        update_run_queue();
    }
}

task_struct* find_process_with_pid(int pid) {
    task_struct *task;
    for(int i=0; i< 2048; ++i) {
        task = s_process_queue[i].cur_task;
        if(task->pid == pid){
            return task;
        }
    }
    return NULL;
}

task_struct* find_first_child(int pid) {
    task_struct *task;
    for(int i=0; i< 2048; ++i) {
        task = s_process_queue[i].cur_task;
        if(task->ppid == pid){
            return task;
        }
    }
    return NULL;
}

int sys_wait(uint64_t status) {
   task_struct* task = find_first_child(s_cur_run_task->pid);
   if(task == NULL) {
       if(task->child_exit_status != -1) {
           // Child already exited. Return immediately
           *(uint64_t *)status = task->child_exit_status;
           return 0;
       }
       kprintf("\nProcess had no child. Returning to parent process !! \n");
       return 0;
   }
   s_cur_run_task->state  = WAITING;
   schedule_next(s_cur_run_task, task);
   return task->child_exit_pid;
}

int sys_waitpid(int pid, uint64_t status, int options) {
   task_struct* task = find_first_child(pid);
   if(task == NULL) {
       if(task->child_exit_status != -1) {
           // Child already exited. Return immediately
           *(uint64_t *)status = task->child_exit_status;
           return 0;
       }
       kprintf("\nProcess had no child. Returning to parent process !! \n");
       return 0;
   }
   s_cur_run_task->state  = WAITING;
   schedule_next(s_cur_run_task, task);
   return task->child_exit_pid;   
}

void sys_kill(int flag, int pid) {
    if(flag == 9){
        kill_task(pid);
    }
    kprintf("\n Successfully killed process: (Process%d, PID: %d)", pid, pid);
    return;
}

void sys_execve(char* path, char*argv, char* env) {
    create_elf_process_from_binary(s_cur_run_task, "rootfs/bin/ls", NULL);
    //create_elf_process("rootfs/bin/ls", NULL);
    //add_new_task_to_run_queue_end(task);
    //sys_yield();
    return;
}

void sys_exit(int status) {
    kprintf(" Invoked Exit : PID %d NPID: %d ", s_cur_run_task->pid, s_cur_run_task->next->pid);
    task_struct *save = s_cur_run_task;
    task_struct* task = find_process_with_pid(save->ppid);
    task->child_exit_status = 1;
    task->child_exit_pid = save->pid;
    sys_yield();
    //printRunQueue();
    kill_task(save->pid);
    //printRunQueue();
    return;
}

void sys_ps() {

    task_struct *trav = s_cur_run_task;
    kprintf("\n \n");
    kprintf("\n====================");
    kprintf("\n| NAME    |    PID |");
    kprintf("\n====================");
    while(trav != NULL) {
        if(trav->pid == 0) {
            kprintf("\n %s     %d", "SBUSH", 0);
        }
        else {
            kprintf("\n %s %d  %d\n", trav->name, trav->pid, trav->pid);
        }
        trav = trav->next;
    }
    kprintf("\n \n");
    kprintf("\n \n");
}

void sys_sleep(int time) {
  uint64_t count = get_pit_count();
  __asm__ __volatile__("sti\n");
  count = count + (4000 * time); 
  while(count > get_pit_count()) {
  }
  return;
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

void first_switch_to(task_struct *cur, task_struct *next);

task_struct* get_next_task_from_run_queue() {
    return s_cur_run_task->next;
}

uint64_t sys_yield() {
    task_struct* cur  = s_cur_run_task;
    task_struct* next = get_next_task_from_run_queue();

    if(next != NULL && cur != next) {
        kprintf(" Will do Context Switch cur(%d) to next(%d) !!!", s_cur_run_task->pid, next->pid);
        set_cr3_register(next->pml4);
        __asm__ __volatile__ ("movq %%cr3,%%rax\n" : : );
        __asm__ __volatile__ ("movq %%rax,%%cr3\n" : : );
        switch_to(cur, next);
        s_cur_run_task = next;
        next->state = RUNNING;
        cur->state  = SLEEPING;
        update_run_queue();
    }
    return s_cur_run_task->pid;
}


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

void test_switch_to_ring3() {

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


void switch_to_ring3(task_struct* cur, task_struct* next) {

    set_tss_rsp((uint64_t *)(cur->kernel_rsp));

    __asm__ __volatile__
    (  "pushq $0x23\n"
       "pushq %0\n"
       "pushfq\n"
       "pushq $0x2b\n"
       :
       :"r" (next->user_rsp)
    );

    __asm__ __volatile__
    (   "pushq %0\n"
        "iretq\n"
        :
        :"r" (next->rip)
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

/*void test_vma_add_node_to_empty_vma_list() {
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
}*/

/*void test_vma_operations_on_bigger_list() {
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
}*/


void test_print_dir_empty_list() {
	print_dir();
}

void test_dir_find_node_from_empty_list() {
	struct dir_info *node = find_dir(30);
    if (node == NULL) {
		kprintf("\nPASS : Node search from empty list is NULL");
	} else {
		kprintf("\nFAIL : Node search from empty list is not NULL");
	}
}


void test_dir_add_node_to_empty_list() {
	if(add_dir(1, NULL)) {
		struct dir_info * node = find_dir(0); // This is first open file
		if (node == NULL) {
			kprintf("\nFAIL : searching node after adding to empty list failed");
		} else {
			if(delete_dir(0)) {
				node = find_dir(0);
				if (node == NULL) {
					kprintf("\nPASS : EmptyList -> Add -> find -> delete -> find");
				} else {
					kprintf("\nFAIL : Deleting node after adding to empty vma list unsuccessful");
				}
			} else {
				kprintf("\nFAIL : Deleting node after adding to empty vma list failed");
			}
		}
	} else {
		kprintf("\nFAIL : Adding node to empty list failed");
	}
}

void test_dir_operations_on_bigger_list() {
     add_dir(1, NULL); // start = 1
     add_dir(2, NULL); // start = 2
     add_dir(3, NULL); // start = 3
     add_dir(4, NULL); // start = 4
     add_dir(5, NULL); // start = 5
     add_dir(5, NULL); // start = 6
     add_dir(4, NULL); // start = 7
     add_dir(3, NULL); // start = 8

     print_dir();
     struct dir_info * node = find_dir(5);
     if (node != NULL) {
         kprintf("\nPASS : Search middle element from list");
     } else {
         kprintf("\nFAIL : Search middle element from list");
     }

     node = find_dir(1);
     if (node != NULL) {
         kprintf("\nPASS : Search last element from list");
     } else {
         kprintf("\nFAIL : Search last element from list");
     }

     node = find_dir(8);
     if (node != NULL) {
         kprintf("\nPASS : Search first element from list");
     } else {
         kprintf("\nFAIL : Search first element from list");
     }

     delete_dir(5);
     node = find_dir(5);
     if (node != NULL) {
         kprintf("\nFAIL : Search middle element from list after delete");
     } else {
         kprintf("\nPASS : Search middle element from list after delete");
     }

     delete_dir(1);
     node = find_dir(1);
     if (node != NULL) {
         kprintf("\nFAIL : Search last element from list after delete");
     } else {
         kprintf("\nPASS : Search last element from list after delete");
     }

     delete_dir(8);
     node = find_dir(8);
     if (node != NULL) {
         kprintf("\nFAIL : Search first element from list after delete");
     } else {
         kprintf("\nPASS : Search first element from list after delete");
     }

     print_dir();
 }

void test_open_dir_for_non_existing_directory() {
	struct dir_info * dirinfo = sys_opendir("rootfs/bin/non_exiting_folder");
	if (dirinfo == NULL) {
		kprintf("\n PASS : opendir for non existing directory");
	} else {
		kprintf("\n FAIL : opendir for non existing directory");
	}
}

void test_open_dir_for_existing_directory() {
	struct dir_info * dirinfo = sys_opendir("rootfs/lib");
	if (dirinfo != NULL) {
		kprintf("\n PASS : opendir for existing directory");
	} else {
		kprintf("\n FAIL : opendir for existing directory");
	}
}

 void test_close_dir_for_non_existing_directory() {
	if (sys_closedir(NULL) == -1) {
	      kprintf("\n PASS : sys_closedir for non existing directory");
	    } else {
	        kprintf("\n FAIL : sys_closedir for non existing directory");
	    }
	}

 void test_close_dir_for_existing_directory() {
	struct dir_info * dirinfo = sys_opendir("rootfs/lib");
	if (sys_closedir(dirinfo) == 0) {
	    kprintf("\n PASS : sys_closedir for existing directory");
	} else {
	    kprintf("\n FAIL : sys_closedir for existing directory");
	}
}

void test_read_dir_for_existing_directory() {
	struct dir_info * dirinfo = sys_opendir("rootfs/lib");
	struct dirent * child = sys_readdir(dirinfo);
	kprintf("\n First child name - %s", child->d_name);
	child = sys_readdir(dirinfo);
	kprintf("\n Second child name - %s", child->d_name);
	child = sys_readdir(dirinfo);
	if (child == NULL) {
		kprintf("\n PASS : sys_readdir");
	} else {
		kprintf("\n FAIL : sys_readdir");
	}
}

void test_file_descriptor_table() {
	kprintf("\n Testing File descriptor table list\n");
	s_cur_run_task = s_init_process;
	test_print_dir_empty_list();
	test_dir_find_node_from_empty_list();
	test_dir_add_node_to_empty_list();
	test_dir_operations_on_bigger_list();
	//print_node_inorder(root_node);
	test_open_dir_for_non_existing_directory();
	test_open_dir_for_existing_directory();
	test_close_dir_for_non_existing_directory();
	test_close_dir_for_existing_directory();
	test_read_dir_for_existing_directory();
}

void test_chdir() {
	kprintf("\n Testing chdir");
	print_node_inorder(root_node);
	s_cur_run_task = s_init_process;
	char result[256];
	sys_getcwd(result, 256);
	kprintf("\n Current working directory is : %s", result);
	sys_chdir("..");
	sys_getcwd(result, 256);
	kprintf("\n Current working directory is : %s", result);

	sys_chdir("rootfs/lib");
	sys_getcwd(result, 256);
	kprintf("\n Current working directory is : %s", result);

	sys_chdir("..");
	sys_getcwd(result, 256);
	kprintf("\n Current working directory is : %s", result);

	sys_chdir("rootfs/lib/libc.a");
	sys_getcwd(result, 256);
	kprintf("\n Current working directory is : %s", result);

}

void test_open_read_close() {
	s_cur_run_task = s_init_process;
	print_node_inorder(root_node);
	int fd = sys_open("rootfs/etc/test_file.txt", 0);
	char buff[240];
	sys_read(fd, buff, 240);
	kprintf("%s", buff);
	sys_close(fd);
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
	//sys_clrscreen();
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
    /*int count = 0;
	while(count < 40) {
		kprintf("\n");
		count = count + 1;
	}*/
	//sys_clrscreen();
	//kprintf("\n writting fresh from here\n");
	//test_chdir();
    //test_terminal();
	//test_file_descriptor_table();
	//test_open_read_close();
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
