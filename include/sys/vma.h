#ifndef _VMA_H__
#define _VMA_H__

#include<sys/types.h>

#define true 1
#define false 0

typedef uint8_t bool;

typedef enum Vma_Type {
    DATA,
    TEXT,
    HEAP
}Vma_Type;

typedef struct vma vma;

struct vma {
    Vma_Type vma_type;
    uint64_t start_addr;
    uint64_t end_addr;
    vma *prev;
    vma *next;
}__attribute__((packed));

vma * find_vma(uint64_t start_addr);
bool add_vma(uint64_t size_of_memory_allocated);
bool delete_vma(uint64_t start_addr);
void print_vma();

#endif
