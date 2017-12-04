#include<sys/vfs.h>
#include<sys/kprintf.h>
#include<sys/elf64.h>
#include<sys/types.h>
#include<sys/memcpy.h>
#include<sys/memset.h>
#include<sys/kern_process.h>

#define true 1
#define false 0

typedef uint8_t bool;

    void parse_elf_and_fill_pcb(Elf64_Ehdr * elf_header, task_struct * elf_task) {
        Elf64_Phdr * prgm_header = (Elf64_Phdr *) ((void *) elf_header + elf_header->e_phoff);

        /*TODO: 1. Load CR3
                2.             
        */

        for (int i = 0; i < elf_header->e_phnum; ++i) {
            // Identify code and data segment
            if ((int)prgm_header->p_type == 1) {
                uint64_t start_va = prgm_header->p_vaddr;
                uint64_t size = prgm_header->p_memsz;
                uint64_t end_va = start_va + size;

                kprintf("\n start - %p and end - %p", start_va, end_va);

                if ((int)prgm_header->p_flags == 5)  { // This is TEXT segment
                    kprintf("\nGot Text segment");
                } else if ((int)prgm_header->p_flags == 6) { // This is Data segment
                    kprintf("\nGot Data segment");
                } else {
                    kprintf("\nGot Unidentified segment");
                }

                /*TODO: 1. Load CR3
                        2. Load ELF sections into new virtual memory area with R/W and R/E permissions            
                */

                // Memcopy bytes to virtual address causing page fault which should be handled
                //memcpy((void *) start_va, (void *) elf_header + prgm_header->p_offset, prgm_header->p_filesz);

                // set bss section with 0
                //memset((void *) start_va + prgm_header->p_filesz, 0, size - prgm_header->p_filesz);                               
            }
            
            prgm_header += 1;
        }
        // Main Function => 0000000000400105

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
        parse_elf_and_fill_pcb(elf_header, elf_task);         
        return elf_task;
    }
