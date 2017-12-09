#include <sys/kprintf.h>
#include <sys/page_table.h>
#include <sys/phys_pages.h>
#include <sys/kmalloc.h>
#include <sys/idt.h>
#include <sys/kern_process.h>

#define USER_BASE   0xFFFFFFFF00000000

extern char kernmem;
extern uint64_t s_cur_page_index;
extern task_struct* s_init_process;
extern task_struct* s_cur_run_task;
uint64_t s_pml4_table;

uint64_t cr3;
uint64_t KB = (uint64_t)KERN_BASE;
uint64_t PAGE_SIZE = 0x1000;
extern uint64_t UB;

uint64_t create_pml4_table(){
    s_pml4_table  = allocate_phys_page();
    return s_pml4_table;
}

uint64_t readCR3() {
    uint64_t addr = 0;
    __asm__ __volatile__
    (  "movq %%cr3, %0\n"
       : "=r"(addr)
       :
    );
    return addr;
}

uint64_t create_pdpt_table(uint64_t pml4Table, uint64_t index, uint8_t user){
    uint64_t pdpt_table  = allocate_phys_page();
    uint64_t entry = pdpt_table;
    uint64_t vaddr_pml4 = pml4Table;
    if( user != 0){
        //kprintf("PDPT Table: %p \n", pdpt_table);
        entry |= (PTE_P | PTE_W | PTE_U);
        vaddr_pml4 = KB + pml4Table;
        //kprintf("PDPT entry with Offset: %p \n", entry);
    }
    else {
        //kprintf("PDPT Table: %p \n", pdpt_table);
        entry |= (PTE_P | PTE_W );
        //kprintf("PDPT entry with Offset: %p \n", entry);
        vaddr_pml4 = KB + pml4Table;
    }
    *(uint64_t *)(vaddr_pml4 + index) = entry;
    //kprintf("PML4 Vaddr: [ %p  %p  %p ]", vaddr_pml4, (uint64_t *)(vaddr_pml4 + index), *(uint64_t *)(vaddr_pml4 + index));
    return pdpt_table;
}

uint64_t create_pd_table(uint64_t pdptTable, uint64_t index, uint8_t user){
    uint64_t pd_table  = allocate_phys_page();
    uint64_t entry = pd_table;
    uint64_t vaddr_pdpt = pdptTable;
    if (user != 0) {
        //kprintf("PD Table: %p \n", pd_table);
        entry |= (PTE_P | PTE_W | PTE_U);
        vaddr_pdpt = KB + pdptTable;
        //kprintf("PD entry with Offset: %p \n", entry);
    }
    else {
        //kprintf("PD Table: %p \n", pd_table);
        entry |= (PTE_P | PTE_W );
        vaddr_pdpt = KB + pdptTable;
        //kprintf("PD entry with Offset: %p \n", entry);
    }
    *(uint64_t *)(vaddr_pdpt + index) = entry;
    //kprintf("PDPT Vaddr: [ %p %p %p ] ", vaddr_pdpt, (uint64_t *)(vaddr_pdpt + index), *(uint64_t *)(vaddr_pdpt + index));
    return pd_table;
}

uint64_t create_pt_table(uint64_t pdTable, uint64_t index, uint8_t user){
    uint64_t pt_table  = allocate_phys_page();
    uint64_t entry = pt_table;
    uint64_t vaddr_pd = pdTable;
    if(user != 0) {
        //kprintf("PT Table: %p ", pt_table);
        entry |= (PTE_P | PTE_W | PTE_U);
        vaddr_pd = KB + pdTable;
        //kprintf("PT entry with Offset: %p \n", entry);
    }
    else {
        //kprintf("PT Table: %p ", pt_table);
        entry |= (PTE_P | PTE_W );
        vaddr_pd = KB + pdTable;
        //kprintf("PT entry with Offset: %p ", entry);
    }
    //kprintf("PD Vaddr: %p Index: %d", (uint64_t)vaddr_pd, index);
    *(uint64_t *)(vaddr_pd + index) = entry;
    return pt_table;
}

void init_kernel_page_table(uint64_t kern_start, uint64_t kern_end, uint64_t
                            phys_page_start, uint64_t phys_page_end){
    uint64_t kern_vaddr_start = (uint64_t)&kernmem;
    uint64_t kern_vaddr_end = (uint64_t)&kernmem - kern_start + kern_end;
    uint64_t pml4Index = PML4_ENTRY_INDEX(kern_vaddr_start);
    uint64_t pdptIndex = PDPT_ENTRY_INDEX(kern_vaddr_start);
    uint64_t pdIndex = PD_ENTRY_INDEX(kern_vaddr_start);
    uint64_t ptIndex = PT_ENTRY_INDEX(kern_vaddr_start);
    kprintf("KS: %p, KE: %p\n", kern_start, kern_end);
    //kprintf("kern_vaddr_start: %p ", kern_vaddr_start);
    //kprintf("kern_vaddr_end:   %p \n", kern_vaddr_end);
    //kprintf("pml4Index: %p ", pml4Index);
    //kprintf("pdptIndex: %p ", pdptIndex);
    //kprintf("PdIndex: %p ", pdIndex);
    kprintf("PtIndex: %p \n", ptIndex);

    uint64_t pml4_table = create_pml4_table();
    cr3 = pml4_table;
    kprintf("PML4: %p ", pml4_table);
    kprintf("CR3:  %p ", cr3);
    if(!s_pml4_table) {
         kprintf("KERNEL PANIC: Can't allocate free page for PML4 table !!!");
         return;
    }
    uint64_t pdpt_table = create_pdpt_table(pml4_table, pml4Index * sizeof(uint64_t), 0);
    kprintf("PDPT: %p ", pdpt_table);
    if(!pdpt_table) {
         kprintf("KERNEL PANIC: Can't allocate free page for PDPT table !!!");
         return;
    }
    uint64_t pd_table = create_pd_table(pdpt_table, pdptIndex * sizeof(uint64_t), 0);
    //kprintf("PD: %p ", (uint64_t)pd_table);
    if(!pd_table) {
         kprintf("KERNEL PANIC: Can't allocate free page for PD table !!!");
         return;
    }
    uint64_t pt_table = create_pt_table(pd_table, pdIndex * sizeof(uint64_t), 0);
    //kprintf("PT: %p \n", (uint64_t)pt_table);
    if(!pt_table) {
         kprintf("KERNEL PANIC: Can't allocate free page for PT table !!!");
         return;
    }

    uint64_t vaddr = kern_vaddr_start;
    uint64_t phyaddr = kern_start;
    uint64_t vaddr_pt = KB + pt_table;
    //kprintf("ROKU: Vaddr: %p ", vaddr);
    for(;vaddr <= kern_vaddr_end; vaddr+= PAGE_SIZE, phyaddr+=PAGE_SIZE) {

        uint64_t ptIndex = PT_ENTRY_INDEX(vaddr);
        uint64_t entry = phyaddr;
        entry |= (PTE_P | PTE_W );
        *(uint64_t *)(vaddr_pt + (sizeof(uint64_t) * ptIndex)) = entry;
    }
    kprintf("\nkS: %p, kE: %p Phys_page_start %p Phys_page_end %p", kern_start,
            kern_end, get_phys_addr(s_cur_page_index), phys_page_end);

    map_vaddr_to_physaddr(VIDEO_BUFFER_BASE_ADDR, VIDEO_BUFFER_BASE_PHYS_ADDR, 1);
    map_free_pages(get_phys_addr(s_cur_page_index), phys_page_end);
    set_cr3_register(cr3);
    kprintf("\n HELLO WORLD !!! \n");
    //enable_Interrupts();
    /*uint64_t addr = allocate_phys_page();
    uint64_t *ptr = (uint64_t *)(KERN_BASE + addr);
    *ptr = 25;
    kprintf("\n Var = %d var addr = %p \n", *ptr, ptr );
    ptr = (uint64_t *)kmalloc(100);
    *ptr = 50;
    kprintf("\n Var = %d var addr = %p \n", *ptr, ptr );
    kfree((void *)ptr);
    ptr = (uint64_t *)kmalloc(100);
    *ptr = 1000;
    kprintf("\n Var = %d var addr = %p \n", *ptr, ptr );*/

    return;
}

void map_free_pages(uint64_t phys_page_start, uint64_t phys_page_end) {
    uint64_t vaddr = KB + phys_page_start;
    uint64_t physaddr = phys_page_start;
    //kprintf("Map vaddr = %p, physaddr = %p \n", vaddr, physaddr);
    //kprintf("Page Index: %d Start = %p, End= %p \n", s_cur_page_index, phys_page_start,  phys_page_end);
    for(;physaddr < phys_page_end; vaddr+= PAGE_SIZE, physaddr+=PAGE_SIZE) {
        map_vaddr_to_physaddr(vaddr, physaddr, 0);
    }
    return;
}

void map_vaddr_to_physaddr(uint64_t vaddr, uint64_t physaddr, uint8_t user) {
    uint64_t pml4Index = PML4_ENTRY_INDEX(vaddr);
    uint64_t pdptIndex = PDPT_ENTRY_INDEX(vaddr);
    uint64_t pdIndex = PD_ENTRY_INDEX(vaddr);
    uint64_t ptIndex = PT_ENTRY_INDEX(vaddr);

    uint64_t pml4_table = s_pml4_table;
    if(!s_pml4_table) {
         kprintf("KERNEL PANIC: PML4 table missing !!!");
         return;
    }

    uint64_t pml4_entry = *(uint64_t *)(pml4_table + (sizeof(uint64_t) * pml4Index));
    //kprintf(" PML4 = %p  ", pml4_entry);
    uint64_t pdpt_table;
    if(pml4_entry & PTE_P) {
        pdpt_table = PAGE_GET_PHYSICAL_ADDRESS(pml4_entry);
        //kprintf(" Pdpt Table = %p  ", (uint64_t)pdpt_table);
    }
    else{
        pdpt_table = create_pdpt_table(pml4_table, (sizeof(uint64_t) * pml4Index), user);
        if(!pdpt_table) {
            kprintf("KERNEL PANIC: Can't allocate free page for PDPT table !!!");
            return;
        }
    }
    uint64_t pdpt_entry = *(uint64_t *)(pdpt_table + (sizeof(uint64_t) * pdptIndex));
    //kprintf(" PDPT = %p  ", pdpt_entry);
    uint64_t pd_table;
    if(pdpt_entry & PTE_P) {
        pd_table = PAGE_GET_PHYSICAL_ADDRESS(pdpt_entry);
        //kprintf(" Pd Table = %p  ", (uint64_t)pd_table);
    }
    else{
        pd_table = create_pd_table(pdpt_table, (sizeof(uint64_t) * pdptIndex), user);
        if(!pd_table) {
            kprintf("kernel panic: can't allocate free page for pd table !!!");
            return;
        }
    }
    uint64_t pd_entry = *(uint64_t *)(pd_table + (sizeof(uint64_t) * pdIndex));
    //kprintf(" PD = %p  ", pd_entry);
    uint64_t pt_table;
    if(pd_entry & PTE_P) {
        pt_table = PAGE_GET_PHYSICAL_ADDRESS(pd_entry);
        //kprintf(" PT table = %p  ", (uint64_t)pt_table);
    }
    else{
        pt_table = create_pt_table(pd_table, (sizeof(uint64_t) * pdIndex), user);
        if(!pt_table) {
            kprintf("kernel panic: can't allocate free page for pd table !!!");
            return;
        }
    }
    uint64_t vaddr_pt = KB + pt_table;
    uint64_t entry = physaddr;
    entry |= (PTE_P | PTE_W | PTE_U);
    //entry |= (PTE_P | PTE_W );
    //kprintf(" Virtual PT table = %p  Entry = %p  ptIndex = %p", (uint64_t)vaddr_pt, (uint64_t)entry, ptIndex);
    *(uint64_t *)(vaddr_pt + (sizeof(uint64_t) * ptIndex)) = entry;
    //kprintf(" Vaddr: %p, Paddr= %p ", vaddr, physaddr);
    return;
}


void invalidate_page_table(uint64_t vaddr)
{
    uint64_t pml4Index = PML4_ENTRY_INDEX(vaddr);
    uint64_t pdptIndex = PDPT_ENTRY_INDEX(vaddr);
    uint64_t pdIndex = PD_ENTRY_INDEX(vaddr);
    uint64_t ptIndex = PT_ENTRY_INDEX(vaddr);
    
    uint64_t pml4_table = s_cur_run_task->pml4;
    uint64_t v_pml4_table = KB + pml4_table;
    uint64_t pml4_entry = *(uint64_t *)(v_pml4_table + (sizeof(uint64_t) * pml4Index));
    uint64_t pdpt_table;
    if(pml4_entry & PTE_P) {
        pdpt_table = PAGE_GET_PHYSICAL_ADDRESS(pml4_entry);
    }
    else{
        return;
    }
    uint64_t v_pdpt_table = KB + pdpt_table;
    uint64_t pdpt_entry = *(uint64_t *)(v_pdpt_table + (sizeof(uint64_t) * pdptIndex));
    uint64_t pd_table;
    if(pdpt_entry & PTE_P) {
        pd_table = PAGE_GET_PHYSICAL_ADDRESS(pdpt_entry);
    }
    else{
        return;
    }
    uint64_t v_pd_table = KB + pd_table;
    uint64_t pd_entry = *(uint64_t *)(v_pd_table + (sizeof(uint64_t) * pdIndex));
    uint64_t pt_table;
    if(pd_entry & PTE_P) {
        pt_table = PAGE_GET_PHYSICAL_ADDRESS(pd_entry);
    }
    else{
        return;
    }
    uint64_t vaddr_pt = KB + pt_table;
    uint64_t pt_entry = *(uint64_t *)(vaddr_pt + (sizeof(uint64_t) * ptIndex));
    if(pt_entry & PTE_P) {
        uint64_t entry = pt_entry;
        entry &= (~PTE_P);
        *(uint64_t *)(vaddr_pt + (sizeof(uint64_t) * ptIndex)) = entry;
        return;
    }
    return;
}



void page_fault_handler(uint64_t vaddr)
{
    uint64_t pml4Index = PML4_ENTRY_INDEX(vaddr);
    uint64_t pdptIndex = PDPT_ENTRY_INDEX(vaddr);
    uint64_t pdIndex = PD_ENTRY_INDEX(vaddr);
    uint64_t ptIndex = PT_ENTRY_INDEX(vaddr);
    
    uint64_t pml4_table = s_cur_run_task->pml4; //s_init_process->pml4);
    //kprintf("\n PML4  (%p) KB: %p s_pml4_table: %p ", pml4_table, KB, s_pml4_table);
    uint64_t v_pml4_table = KB + pml4_table;
    uint64_t pml4_entry = *(uint64_t *)(v_pml4_table + (sizeof(uint64_t) * pml4Index));
    uint64_t pdpt_table;
    if(pml4_entry & PTE_P) {
        pdpt_table = PAGE_GET_PHYSICAL_ADDRESS(pml4_entry);
        uint64_t entry = pml4_entry;
        entry |= (PTE_P | PTE_W | PTE_U);
        *(uint64_t *)(v_pml4_table + (sizeof(uint64_t) * pml4Index)) = entry;
        //kprintf(" Pdpt Table = %p, Index: %d ", pdpt_table, pdptIndex);
    }
    else{
        pdpt_table = create_pdpt_table(pml4_table, (sizeof(uint64_t)* pml4Index), 1);
        if(!pdpt_table) {
            kprintf("KERNEL PANIC: PAGE FAULT HANDLER: Can't allocate free page for PDPT table !!!");
            return;
        }
    }
    uint64_t v_pdpt_table = KB + pdpt_table;
    uint64_t pdpt_entry = *(uint64_t *)(v_pdpt_table + (sizeof(uint64_t) * pdptIndex));
    //kprintf(" PDPT = %p  ", pdpt_entry);
    uint64_t pd_table;
    if(pdpt_entry & PTE_P) {
        pd_table = PAGE_GET_PHYSICAL_ADDRESS(pdpt_entry);
        uint64_t entry = pdpt_entry;
        entry |= (PTE_P | PTE_W | PTE_U);
        *(uint64_t *)(v_pdpt_table + (sizeof(uint64_t) * pdptIndex)) = entry;
        //kprintf(" Pd Table = %p  ", pd_table);
    }
    else{
        pd_table = create_pd_table(pdpt_table, (sizeof(uint64_t) * pdptIndex), 1);
        if(!pd_table) {
            kprintf("kernel panic:  PAGE FAULT HANDLER: can't allocate free page for pd table !!!");
            return;
        }
    }
    uint64_t v_pd_table = KB + pd_table;
    uint64_t pd_entry = *(uint64_t *)(v_pd_table + (sizeof(uint64_t) * pdIndex));
    //kprintf(" PD = %p  ", pd_entry);
    uint64_t pt_table;
    if(pd_entry & PTE_P) {
        pt_table = PAGE_GET_PHYSICAL_ADDRESS(pd_entry);
        uint64_t entry = pd_entry;
        entry |= (PTE_P | PTE_W | PTE_U);
        *(uint64_t *)(v_pd_table + (sizeof(uint64_t) * pdIndex)) = entry;
        //kprintf(" PT table = %p  ", pt_table);
    }
    else{
        pt_table = create_pt_table(pd_table, (sizeof(uint64_t) * pdIndex), 1);
        if(!pt_table) {
            kprintf("kernel panic: PAGE FAULT HANDLER: can't allocate free page for pd table !!!");
            return;
        }
    }
    uint64_t vaddr_pt = KB + pt_table;
    uint64_t pt_entry = *(uint64_t *)(vaddr_pt + (sizeof(uint64_t) * ptIndex));
    //kprintf(" PTI : %d  [%p - %p] \n", ptIndex, vaddr_pt, pt_entry);
    if(pt_entry & PTE_P) {
        uint64_t entry = pt_entry;
        entry |= (PTE_P | PTE_W | PTE_U);
        *(uint64_t *)(vaddr_pt + (sizeof(uint64_t) * ptIndex)) = entry;
        return;
    }
    else{
        uint64_t entry = allocate_phys_page();
        entry |= (PTE_P | PTE_W | PTE_U );
        *(uint64_t *)(vaddr_pt + (sizeof(uint64_t) * ptIndex)) = entry;
        //while(1) {}
    }
    return;
}

void flush_tlb_entry(uint64_t addr){
    __asm__ __volatile__("cli");
    __asm__ __volatile__( "invlpg (%0)" : : "b"(addr) : "memory" );
    __asm__ __volatile__("sti");
    return;
}

void set_cr3_register(uint64_t addr){
    uint64_t pml4_addr = (uint64_t) addr;
    __asm__ __volatile__( "movq %0, %%cr3" : :"r"(pml4_addr) );
    return;
}

uint64_t convert_virtual_to_phys(uint64_t vaddr) {
   return (uint64_t) ( vaddr & ~(KB));
}

uint64_t convert_phys_to_virtual(uint64_t physaddr) {
    return (uint64_t)(KB + physaddr);
}


uint64_t readErrorCode() {
    uint64_t errorCode = 0;
    __asm__ __volatile__
    (  "movq %%rsp, %0\n"
       : "=r"(errorCode)
       :
    );
    return errorCode;
}

uint64_t readCR2() {
    uint64_t addr = 0;
    __asm__ __volatile__
    (  "movq %%cr2, %0\n"
       : "=r"(addr)
       :
    );
    return addr;
}

