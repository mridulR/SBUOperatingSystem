#ifndef _PAGE_TABLE_H
#define _PAGE_TABLE_H

#include <sys/types.h>

#define PAGE_MAPPING_LEVEL4_ENTRY_COUNT          512
#define PAGE_DIRECTORY_POINTER_TABLE_ENTRY_COUNT 512
#define PAGE_DIRECTORY_ENTRY_COUNT               512
#define PAGE_TABLE_ENTRY_COUNT                   512

#define PAGE_SIZE   0x1000
#define PTE_P       0x001   // Present Bit
#define PTE_W       0x002   // Writeable Bit
#define PTE_U       0x004   // User Bit                                       
#define PTE_PWT     0x008   // Write Through Bit                                       
#define PTE_PCD     0x010   // Cache Bit
#define PTE_A       0x020   // Accessed Bit
#define PTE_D       0x040   // Dirty Bit                                       
#define PTE_PS      0x080   // Page Size                                           

#define PAGE_ATTR_INDEX(addr)         (addr & 0xFFF)
#define PT_ENTRY_INDEX(addr)          ((addr >> 12) & 0x1FF)
#define PD_ENTRY_INDEX(addr)          ((addr >> 21) & 0x1FF)
#define PDPT_ENTRY_INDEX(addr)        ((addr >> 30) & 0x1FF)
#define PML4_ENTRY_INDEX(addr)        ((addr >> 39) & 0x1FF)
#define PAGE_GET_PHYSICAL_ADDRESS(x)  ((x) & ~0xFFF)

#define KERN_BASE 0xffffffff80000000

// Page Mapping level 4
struct PML4 {
    uint64_t pml4_entries[PAGE_MAPPING_LEVEL4_ENTRY_COUNT];
}__attribute__((aligned(PAGE_SIZE)));

// Page directory pointer table
struct PDPT {
    uint64_t pdpt_entries[PAGE_DIRECTORY_POINTER_TABLE_ENTRY_COUNT];
}__attribute__((aligned(PAGE_SIZE)));


// Page directory table 
struct PD {
    uint64_t pd_entries[PAGE_DIRECTORY_ENTRY_COUNT];
}__attribute__((aligned(PAGE_SIZE)));

// Page table entry
struct PT {
    uint64_t pt_entries[PAGE_TABLE_ENTRY_COUNT];
}__attribute__((aligned(PAGE_SIZE)));

typedef struct PML4 PML4;
typedef struct PDPT PDPT;
typedef struct PD PD;
typedef struct PT PT;

// Initializes the Page table
void init_kernel_page_table(uint64_t kern_start, uint64_t kern_end, uint64_t
                            phys_page_start, uint64_t phys_page_end);

// Flushes the entry in the page table by setting the present bit to 0
void flush_tlb_entry(void *addr);

// Set the cr3 register 
void set_cr3_register(PML4 *addr);

PML4* create_pml4_table();

PDPT* create_pdpt_table(PML4* pml4Table, uint16_t index);

PD* create_pd_table(PDPT* pdptTable, uint16_t index);

PT* create_pt_table(PD* pdTable, uint16_t index);

void map_vaddr_to_physaddr(uint64_t vaddr, uint64_t physaddr);

void map_free_pages(uint64_t phys_page_start, uint64_t phys_page_end);

void* convert_virtual_to_phys(uint64_t vaddr);

void* convert_phys_to_virtual(uint64_t physaddr);

#endif
