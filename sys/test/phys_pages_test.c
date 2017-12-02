#include <sys/phys_pages.h>
#include <sys/types.h>
#include <sys/test/test.h>

#define true 1
#define false 0

#define PHYS_HIGH_BASE_ADDR    0x100000
#define PHYS_HIGH_LIMIT_ADDR   0x6066000
#define PHYS_LOW_BASE_ADDR     0x0
#define PHYS_LOW_LIMIT_ADDR    0x9F000

#define PHYS_HIGH_PAGE_COUNT (((uint64_t)PHYS_HIGH_LIMIT_ADDR - (uint64_t)PHYS_HIGH_BASE_ADDR)/PHYS_PAGE_SIZE)
#define PHYS_LOW_PAGE_COUNT  (((uint64_t)PHYS_LOW_LIMIT_ADDR  - (uint64_t)PHYS_LOW_BASE_ADDR)/PHYS_PAGE_SIZE)
#define PHYS_PAGE_COUNT (PHYS_HIGH_PAGE_COUNT + PHYS_LOW_PAGE_COUNT)

#define PHYS_PAGE_SIZE 4096
#define PAGE_SIZE  0x1000


typedef uint8_t bool;
typedef struct phys_page Phys_page;

extern Phys_page s_phys_page[];
extern uint64_t s_free_page_count;
extern uint64_t s_max_page_count;
extern uint64_t s_cur_page_index;
extern uint64_t s_phys_base_addr;
extern uint64_t s_phys_limit_addr;
extern uint64_t s_kern_start;
extern uint64_t s_kern_end;
extern char kernmem;

void test_physical_pages(uint64_t kern_start, uint64_t kern_end) { }

uint64_t getIndex(uint64_t addr) {
    return (uint64_t)(addr - s_kern_end)/(uint64_t) PAGE_SIZE;
}

void test_allocate_deallocate_page() {
    if(s_free_page_count == 0) {
         kprintf("KERNEL PANIC: Out of Memory !!!\n");
         return;
    }

    for(int i = 0; i<10; ++i) {
        kprintf(" [%d] - (%d, %p) ", i, s_phys_page[i].nextIndex, s_phys_page[i].cur_addr);
    }

    kprintf("\n +++  %d \n\n", s_cur_page_index);
    uint64_t addr0 = allocate_phys_page();
    kprintf("0: (%d, %p) ", getIndex(addr0), addr0);

    uint64_t addr1 = allocate_phys_page();
    kprintf("1: (%d, %p) ", getIndex(addr1), addr1);

    uint64_t addr2 = allocate_phys_page();
    kprintf("2: (%d, %p) ", getIndex(addr2), addr2);

    uint64_t addr3 = allocate_phys_page();
    kprintf("3: (%d, %p) ", getIndex(addr3), addr3);

    uint64_t addr4 = allocate_phys_page();
    kprintf("4: (%d, %p) ", getIndex(addr4), addr4);

    uint64_t addr5 = allocate_phys_page();
    kprintf("5: (%d, %p) ", getIndex(addr5), addr5);

    uint64_t addr6 = allocate_phys_page();
    kprintf("6: (%d, %p) ", getIndex(addr6), addr6);

    kprintf("\n\n ---  %d \n", s_cur_page_index);
    deallocate_phys_page(addr0);
    deallocate_phys_page(addr1);
    deallocate_phys_page(addr2);
    deallocate_phys_page(addr3);
    deallocate_phys_page(addr4);
    deallocate_phys_page(addr5);
    deallocate_phys_page(addr6);

    for(int i = 0; i<10; ++i) {
        kprintf(" [%d] - (%d, %p) ", i, s_phys_page[i].nextIndex, s_phys_page[i].cur_addr);
    }

    kprintf("\n +++  %d \n", s_cur_page_index);
    addr0 = allocate_phys_page();
    kprintf("0': (%d, %p) ", getIndex(addr0), addr0);

    addr1 = allocate_phys_page();
    kprintf("1': (%d, %p) ", getIndex(addr1), addr1);

    addr2 = allocate_phys_page();
    kprintf("2': (%d, %p) ", getIndex(addr2), addr2);

    addr3 = allocate_phys_page();
    kprintf("3': (%d, %p) ", getIndex(addr3), addr3);

    addr4 = allocate_phys_page();
    kprintf("4': (%d, %p) ", getIndex(addr4), addr4);

    addr5 = allocate_phys_page();
    kprintf("5': (%d, %p) ", getIndex(addr5), addr5);

    addr6 = allocate_phys_page();
    kprintf("6': (%d, %p) ", getIndex(addr6), addr6);

    deallocate_phys_page(addr0);
    deallocate_phys_page(addr1);
    deallocate_phys_page(addr2);
    deallocate_phys_page(addr3);
    deallocate_phys_page(addr4);
    deallocate_phys_page(addr5);
    deallocate_phys_page(addr6);

    kprintf("\n ---  %d \n", s_cur_page_index);
    for(int i = 0; i<10; ++i) {
        kprintf(" [%d] - (%d, %p) ", i, s_phys_page[i].nextIndex, s_phys_page[i].cur_addr);
    }
    kprintf("\n ---  %d \n", s_cur_page_index);
}

