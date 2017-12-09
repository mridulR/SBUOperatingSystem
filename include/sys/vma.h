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
uint64_t add_vma(uint64_t size_of_memory_allocated);
bool create_add_vma(uint64_t start_addr, uint64_t end_addr, Vma_Type type);
bool delete_vma(uint64_t start_addr);
void print_vma();
uint64_t find_first_free_vma(uint64_t size);
uint8_t check_vma_access(uint64_t addr);
vma * copy_vma_node(vma* copy_node);
vma * copy_vma_list(vma* copy_root);

#endif
