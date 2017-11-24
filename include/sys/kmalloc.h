#ifndef _KMALLOC_H
#define _KMALLOC_H

#include <sys/types.h>

void * kmalloc(uint64_t vaddr);

void kfree(void* vaddr);

#endif
