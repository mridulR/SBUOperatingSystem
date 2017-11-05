#include <sys/kprintf.h>
#include <sys/page_table.h>
#include <sys/phys_pages.h>

#define PAGE_SIZE 0x1000

extern char kernmem;

PML4 * s_pml4_table;

uint64_t cr3;

PML4* create_pml4_table(){
    s_pml4_table  = (PML4 *)allocate_phys_page();
    return s_pml4_table;
}

PDPT* create_pdpt_table(PML4* pml4Table, uint16_t index, uint64_t kern_vaddr_base){
    PDPT* pdpt_table  = (PDPT *)allocate_phys_page();
    uint64_t entry = (uint64_t)pdpt_table;    
    //kprintf("PDPT Table: %p \n", (uint64_t)pdpt_table);
    entry |= (PTE_P | PTE_W | PTE_U);
    //entry = ((entry << 12) | (PTE_P | PTE_W | PTE_U));
    //kprintf("PDPT entry with Offset: %p \n", (uint64_t)entry);
    uint64_t vaddr_pml4 = kern_vaddr_base + (uint64_t)pml4Table;
    kprintf("PML4 Vaddr: %p ", (uint64_t)vaddr_pml4);
    ((PML4*)vaddr_pml4)->pml4_entries[index] = (uint64_t) entry;
    return pdpt_table;
}

PD* create_pd_table(PDPT* pdptTable, uint16_t index, uint64_t kern_vaddr_base){
    PD* pd_table  = (PD *)allocate_phys_page();
    //kprintf("PD Table: %p \n", (uint64_t)pd_table);
    uint64_t entry = (uint64_t)pd_table;    
    entry |= (PTE_P | PTE_W | PTE_U);
    //entry = ((entry << 12) | (PTE_P | PTE_W | PTE_U));
    //kprintf("PD entry with Offset: %p \n", (uint64_t)entry);
    uint64_t vaddr_pdpt = kern_vaddr_base + (uint64_t)pdptTable;
    kprintf("PDPT Vaddr: %p ", (uint64_t)vaddr_pdpt);
    ((PDPT*)vaddr_pdpt)->pdpt_entries[index] = (uint64_t) entry;
    return pd_table;
}

PT* create_pt_table(PD* pdTable, uint16_t index, uint64_t kern_vaddr_base){
    PT* pt_table  = (PT *)allocate_phys_page();
    //kprintf("PT Table: %p \n", (uint64_t)pt_table);
    uint64_t entry = (uint64_t)pt_table;    
    entry |= (PTE_P | PTE_W | PTE_U);
    //entry = ((entry << 12) | (PTE_P | PTE_W | PTE_U));
    //kprintf("PT entry with Offset: %p \n", (uint64_t)entry);
    uint64_t vaddr_pd = kern_vaddr_base + (uint64_t)pdTable;
    kprintf("PD Vaddr: %p ", (uint64_t)vaddr_pd);
    ((PD*)vaddr_pd)->pd_entries[index] = (uint64_t) entry;
    return pt_table;
}

void init_kernel_page_table(uint64_t kern_start, uint64_t kern_end){
    uint64_t kern_vaddr_start = (uint64_t)&kernmem;
    uint64_t kern_vaddr_base = (uint64_t)&kernmem - kern_start;
    uint64_t kern_vaddr_end = (uint64_t)&kernmem - kern_start + kern_end;
    uint16_t pml4Index = PML4_ENTRY_INDEX(kern_vaddr_start);
    uint16_t pdptIndex = PDPT_ENTRY_INDEX(kern_vaddr_start);
    uint16_t pdIndex = PD_ENTRY_INDEX(kern_vaddr_start);
    uint16_t ptIndex = PT_ENTRY_INDEX(kern_vaddr_start);
    kprintf("KS: %p, KE: %p\n", kern_start, kern_end);
    kprintf("kern_vaddr_start: %p ", kern_vaddr_start);
    kprintf("kern_vaddr_end:   %p \n", kern_vaddr_end);
    kprintf("pml4Index: %p ", pml4Index);
    kprintf("pdptIndex: %p ", pdptIndex);
    kprintf("PdIndex: %p ", pdIndex);
    kprintf("PtIndex: %p \n", ptIndex);

    PML4* pml4_table = create_pml4_table();
    cr3 = (uint64_t) pml4_table;
    kprintf("PML4: %p ", (uint64_t)pml4_table);
    kprintf("CR3:  %p ", cr3);
    if(!s_pml4_table) {
         kprintf("KERNEL PANIC: Can't allocate free page for PML4 table !!!");
         return;
    }
    PDPT* pdpt_table = create_pdpt_table(pml4_table, pml4Index, kern_vaddr_base);
    kprintf("PDPT: %p ", (uint64_t)pdpt_table);
    if(!pdpt_table) {
         kprintf("KERNEL PANIC: Can't allocate free page for PDPT table !!!");
         return;
    }
    PD* pd_table = create_pd_table(pdpt_table, pdptIndex, kern_vaddr_base);
    kprintf("PD: %p ", (uint64_t)pd_table);
    if(!pd_table) {
         kprintf("KERNEL PANIC: Can't allocate free page for PD table !!!");
         return;
    }
    PT* pt_table = create_pt_table(pd_table, pdIndex, kern_vaddr_base);
    kprintf("PT: %p \n", (uint64_t)pt_table);
    if(!pt_table) {
         kprintf("KERNEL PANIC: Can't allocate free page for PT table !!!");
         return;
    }
    
    uint64_t vaddr = kern_vaddr_start;
    uint64_t phyaddr = kern_start;
    uint64_t vaddr_pt = kern_vaddr_base + (uint64_t)pt_table;
    kprintf("PT Vaddr: %p ", (uint64_t)vaddr_pt);
    for(;vaddr <= kern_vaddr_end; vaddr+= PAGE_SIZE, phyaddr+=PAGE_SIZE) {
        uint16_t ptIndex = PT_ENTRY_INDEX(vaddr); 
        uint64_t entry = (uint64_t)phyaddr;    
        entry |= (PTE_P | PTE_W | PTE_U);
        //entry = ((entry << 12) | (PTE_P | PTE_W | PTE_U));
        kprintf(" e= %p ",entry);
        ((PT*)vaddr_pt)->pt_entries[ptIndex] = (uint64_t)entry;
    }
    kprintf("\nPML4[%d]: %p, PDPT[%d]: %p, PD[%d]: %p", pml4Index,
            pml4_table->pml4_entries[pml4Index], pdptIndex,
            pdpt_table->pdpt_entries[pdptIndex], pdIndex,
            pd_table->pd_entries[pdIndex]);
    kprintf("\nkS: %p, kE: %p", kern_start, kern_end);
    set_cr3_register((PML4 *)cr3);
    return;
}

void flush_tlb_entry(void *addr){
    __asm__ __volatile__("cli");
    __asm__ __volatile__( "invlpg (%0)" : : "b"(addr) : "memory" );
    __asm__ __volatile__("sti");
    return;
}

void set_cr3_register(PML4 *addr){
    uint64_t pml4_addr = (uint64_t) addr;
    __asm__ __volatile__( "movq %0, %%cr3" : :"r"(pml4_addr) );
    return;
}

