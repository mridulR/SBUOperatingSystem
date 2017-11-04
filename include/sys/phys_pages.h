#ifndef _PHYS_PAGES_H
#define _PHYS_PAGES_H 

#include <sys/types.h>
#include <sys/kprintf.h>

struct phys_page {
    struct phys_page *next;
} __attribute__((packed));

typedef struct phys_page Phys_page;

void init_phys_page(uint32_t *modulep, uint64_t phybase, uint64_t phyfree);

uint64_t get_phys_addr(uint64_t index);

uint64_t get_first_free_page();

uint64_t allocate_phys_page();

void deallocate_phys_page(uint64_t addr);

#endif
