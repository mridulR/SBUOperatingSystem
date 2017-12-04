#ifndef _KMALLOC_H
#define _KMALLOC_H

#include <sys/types.h>

uint64_t kmalloc(uint64_t vaddr);

void kfree(uint64_t vaddr);

#endif
