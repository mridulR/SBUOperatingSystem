#include<sys/kern_process.h>
#include<sys/kprintf.h>
#include<sys/types.h>
#include<sys/kmalloc.h>
#include<sys/memset.h>
#include<sys/vma.h>

#define PAGE_SIZE  4096

extern task_struct* s_cur_run_task;
extern uint64_t KB;

void print_vma() {
    vma * root = s_cur_run_task->vma_root;
    if (root == NULL) {
        kprintf("\nRoot node is NULL");
    } else {
        vma * trav = root;
        kprintf("\nVMA List is : \n");
        while (trav != NULL) {
            kprintf("%d ", trav->start_addr);
            trav = trav->next;
        }
    }
}

vma * find_vma(uint64_t start_addr) {
    if (s_cur_run_task == NULL) {
        kprintf("\nKernel Panic : No current running process");
        return NULL;
    }
    
    vma * trav = s_cur_run_task->vma_root;
    if (trav == NULL) {
        kprintf("\nVMA list empty");
        return NULL;
    }
    while (trav != NULL) {
        if (trav->start_addr == start_addr) {
            return trav;
        }
        trav = trav->next;
    }
    return NULL;
}

vma * build_vma_node(uint64_t heap_top, uint64_t size_of_memory_allocated) {
    uint64_t addr = KB + kmalloc(sizeof(vma));
    vma * curr = (vma *)addr;
    memset((uint8_t *)curr, '\0', PAGE_SIZE);
    curr->start_addr = heap_top;
    curr->end_addr = heap_top + size_of_memory_allocated;
    curr->vma_type = HEAP;
    curr->next = NULL;
    curr->prev = NULL;
    return curr;    
}


// After adding vma -> caller should set vma_type by calling find with start address
uint64_t add_vma(uint64_t size_of_memory_allocated) {
    uint64_t ret = 0xFFFFFFFFFFFFFFFF;
    if (s_cur_run_task == NULL) {
        kprintf("\nKernel Panic : No current running process");
        return 0;
    }
    
    vma * trav = (vma *)s_cur_run_task->vma_root;
    vma * curr = build_vma_node(s_cur_run_task->heap_top, size_of_memory_allocated);
    ret = (ret & curr->start_addr);
    s_cur_run_task->heap_top = curr->end_addr;
    curr->next = trav;
    s_cur_run_task->vma_root = curr;
    if (trav != NULL) {
        trav->prev = curr;
    }
    return ret;
}

// After adding vma -> caller should set vma_type by calling find with start address
bool create_add_vma(uint64_t start_addr, uint64_t end_addr, Vma_Type type) {
    if (s_cur_run_task == NULL) {
        kprintf("\nKernel Panic : No current running process");
        return false;
    }
    if ( type == HEAP) {
        kprintf("Wrong Invocation to map HEAP\n");
        return false;
    }
    vma * trav = (vma *)s_cur_run_task->vma_root;

    uint64_t addr = KB + kmalloc(sizeof(vma));
    vma * curr = (vma *)addr;
    memset((uint8_t *)curr, '\0', PAGE_SIZE);
    curr->start_addr = start_addr;
    curr->end_addr = end_addr;
    curr->vma_type = type;
    curr->next = NULL;
    curr->prev = NULL;

    curr->next = trav;
    s_cur_run_task->vma_root = curr;
    if (trav != NULL) {
        trav->prev = curr;
    }

    return true;
}

bool delete_vma(uint64_t start_addr) {
    vma * vma_entry = find_vma(start_addr);
    if (vma_entry == NULL) {
        kprintf("VMA entry doesn't exists to be deleted");
        return false;
    }
    vma * trav = s_cur_run_task->vma_root;

    // if first node is to be deleted
    if (trav->start_addr == start_addr) {
        vma * next = trav->next;
        if (next != NULL) {
            next->prev = NULL;
        }
        s_cur_run_task->vma_root = next;
        kfree((uint64_t)vma_entry);
        return true;
    }

    while (trav->next->start_addr != vma_entry->start_addr) {
        trav = trav->next;
    }
    trav->next = vma_entry->next;
    if (vma_entry->next != NULL) {
        vma_entry->next->prev = trav;
    }
    kfree((uint64_t)vma_entry);
    return true;
}
