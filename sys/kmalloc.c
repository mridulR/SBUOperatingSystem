#include <sys/kmalloc.h>
#include <sys/kprintf.h>
#include <sys/phys_pages.h>
#include <sys/page_table.h>

//! mallocs only single page
void * kmalloc(uint64_t size) {
    if(size == 0) {
        return NULL;
    }
    
    if(size > 0x1000) {
         kprintf("KERNEL PANIC: Kmalloc requested page size too big !!!");
    }
    return (void *)((uint64_t)KERN_BASE + allocate_phys_page());
}


void kfree(void* vaddr) {
    uint64_t addr = (uint64_t)convert_virtual_to_phys((uint64_t)vaddr);
    deallocate_phys_page(addr);
}
