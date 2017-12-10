#ifndef _PAGE_TABLE_H
#define _PAGE_TABLE_H

#include <sys/types.h>

#define PAGE_MAPPING_LEVEL4_ENTRY_COUNT          512
#define PAGE_DIRECTORY_POINTER_TABLE_ENTRY_COUNT 512
#define PAGE_DIRECTORY_ENTRY_COUNT               512
#define PAGE_TABLE_ENTRY_COUNT                   512

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

extern uint64_t PAGE_SIZE;

// Initializes the Page table
void init_kernel_page_table(uint64_t kern_start, uint64_t kern_end, uint64_t
                            phys_page_start, uint64_t phys_page_end);

// Flushes the entry in the page table by setting the present bit to 0
void flush_tlb_entry(uint64_t addr);

// Set the cr3 register 
void set_cr3_register(uint64_t addr);

uint64_t create_pml4_table();

uint64_t create_pdpt_table(uint64_t pml4Table, uint64_t index, uint8_t user);

uint64_t create_pd_table(uint64_t pdptTable, uint64_t index, uint8_t user);

uint64_t create_pt_table(uint64_t pdTable, uint64_t index, uint8_t user);

void map_vaddr_to_physaddr(uint64_t vaddr, uint64_t physaddr, uint8_t user);

void user_map_vaddr_to_physaddr(uint64_t vaddr, uint64_t physaddr, uint8_t user);

void map_free_pages(uint64_t phys_page_start, uint64_t phys_page_end);

uint64_t convert_virtual_to_phys(uint64_t vaddr);

uint64_t convert_phys_to_virtual(uint64_t physaddr);

uint64_t readCR2();

uint64_t readErrorCode();

void page_fault_handler(uint64_t vaddr);

#endif
