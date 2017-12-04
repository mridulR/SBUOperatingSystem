#include <sys/kmalloc.h>
#include <sys/kprintf.h>
#include <sys/phys_pages.h>
#include <sys/page_table.h>

//! mallocs only single page
uint64_t kmalloc(uint64_t size) {
    if(size == 0) {
        return 0;
    }
    
    if(size > 0x1000) {
         kprintf("KERNEL PANIC: Kmalloc requested page size too big !!!");
    }
    return allocate_phys_page();
}


void kfree(uint64_t vaddr) {
    uint64_t addr = convert_virtual_to_phys(vaddr);
    deallocate_phys_page(addr);
}
