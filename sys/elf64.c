#include<sys/vfs.h>
#include<sys/kprintf.h>
#include<sys/elf64.h>
#include<sys/types.h>
#include<sys/memcpy.h>
#include<sys/memset.h>
#include<sys/kern_process.h>
#include<sys/page_table.h>
#include<sys/gdt.h>

#define true 1
#define false 0

typedef uint8_t bool;
extern task_struct* s_init_process;
extern task_struct* s_cur_run_task;

extern uint64_t PS;

void my_switch_to_ring3(task_struct *elf_task) {
    set_tss_rsp((uint64_t *)elf_task->kernel_rsp);
    __asm__ __volatile__
    (  "pushq $0x23\n"
       "pushq %0\n"
       "pushfq\n"
       "pushq $0x2b\n"
       :
       :"r" (elf_task->user_rsp)
    );

    __asm__ __volatile__ 
    (   "pushq %0\n"
        "iretq\n"
        :
        :"r" (elf_task->entry_addr)
    );

}


void map_process_address_space(uint64_t start, uint64_t size) {
   while(size > 0){
       page_fault_handler(start);
       start = start + 1;
       size = size -1;
   }
   return;
}


void parse_elf_and_fill_pcb(Elf64_Ehdr * elf_header, task_struct * elf_task) {
    Elf64_Phdr * prgm_header = (Elf64_Phdr *) ((void *) elf_header + elf_header->e_phoff);

    // Load CR3
    set_cr3_register(elf_task->pml4);
    
    // Main Function => 0000000000400105
    uint64_t entry_addr = 0x00000000004000cd;
    //uint64_t entry_addr = (uint64_t)elf_header->e_entry;
    kprintf("\n Entry of main is - %p \n", (uint64_t)elf_header->e_entry);

    elf_task->entry_addr = entry_addr;

    for (int i = 0; i < elf_header->e_phnum; ++i) {
        // Identify code and data segment
        if ((int)prgm_header->p_type == 1) {
            uint64_t start_va = prgm_header->p_vaddr;
            uint64_t size = prgm_header->p_memsz;
            uint64_t end_va = start_va + size;

            //kprintf("\n start - %p and end - %p", start_va, end_va);
            //kprintf("\n PGM Hdr - %p ",  (elf_header + prgm_header->p_offset));

            if ((int)prgm_header->p_flags == 5)  {
                // This is TEXT segment
                // kprintf("\nGot Text segment");
                map_process_address_space(start_va, size);
            } 
            else if ((int)prgm_header->p_flags == 6) { 
                // This is Data segment
                // kprintf("\nGot Data segment");
                map_process_address_space(start_va, size);
            } 
            else {
                kprintf("\nKERNEL PANIC: Got Unidentified segment [start_va: %p, end_va: %p]", start_va, end_va);
            }
        }
        prgm_header += 1;
    }
    //map_process_address_space(elf_task->user_rsp, 4096);
    // Reset prgm header
    prgm_header = (Elf64_Phdr *) ((void *) elf_header + elf_header->e_phoff);
    
    // Flushing TLB  
    __asm__ __volatile__ ("movq %%cr3,%%rax\n" : : );
    __asm__ __volatile__ ("movq %%rax,%%cr3\n" : : );
    
    for (int i = 0; i < elf_header->e_phnum; ++i) {
        // Identify code and data segment
        if ((int)prgm_header->p_type == 1) {
            uint64_t start_va = prgm_header->p_vaddr;
            uint64_t size = prgm_header->p_memsz;
            uint64_t end_va = start_va + size;

            //kprintf("\n start - %p and end - %p", start_va, end_va);
            //kprintf("\n PGM Hdr - %p ",  (elf_header + prgm_header->p_offset));

            if ((int)prgm_header->p_flags == 5) { 
                // This is TEXT segment
                // Memcopy bytes to virtual address causing page fault which should be handled
                memcpy((uint64_t *)start_va, (uint64_t *) (elf_header + prgm_header->p_offset), size);
                //memcpy((uint64_t *)start_va, (uint64_t *) (elf_header + prgm_header->p_offset), prgm_header->p_filesz);
                //kprintf("\nGot Text segment");
            } 
            else if ((int)prgm_header->p_flags == 6) { 
                // This is Data segment
                // kprintf("\nGot Data segment");
                // Memcopy bytes to virtual address causing page fault which should be handled
                memcpy((uint64_t *)start_va, (uint64_t *) (elf_header + prgm_header->p_offset), size);
                //memcpy((uint64_t *)start_va, (uint64_t *) (elf_header + prgm_header->p_offset), prgm_header->p_filesz);
            } 
            else {
                kprintf("\nKERNEL PANIC: Got Unidentified segment [start_va: %p, end_va: %p]", start_va, end_va);
            }
        }
        prgm_header += 1;
    }

    // Switch to ring3 
    my_switch_to_ring3(elf_task);
    kprintf(" Test: in Elf64.c");

}


bool is_elf_file(Elf64_Ehdr * elf_header) {
    if (elf_header == NULL) {
        return false;
    }

    if(elf_header->e_ident[1] == 'E' 
        && elf_header->e_ident[2] == 'L' && elf_header->e_ident[3] == 'F') {
        return true;
    }
    
    return false;
}



task_struct * create_elf_process(char * file_name, char *argv[]) {
    
    v_file_node* vfs_node = search_file(file_name, tarfs_mount_node);
    if (vfs_node == NULL) {
        kprintf("No ELF file found at location : %s", file_name);
        return NULL;
    }

    Elf64_Ehdr * elf_header = (Elf64_Ehdr *) vfs_node->start_addr;

    if (!is_elf_file(elf_header)) {
        kprintf("Elf_header not found for : %s", file_name);
        return NULL;
    }

    task_struct * elf_task = create_task(0);  // setting parent process id to init
    s_cur_run_task = elf_task;
    parse_elf_and_fill_pcb(elf_header, elf_task);         
    return elf_task;
}
