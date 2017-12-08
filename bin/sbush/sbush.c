#include <stdio.h>
#include <stdlib.h>
#include "syscall.h"

int main(int argc, char *argv[], char *envp[]) {
    
    char buf[1024];
    memset(buf, '\0', 1024); 
    while(1) {
      puts("\nsbush~>");
      gets(buf);
      puts(buf);
    }

    mmap((uint64_t *)0xFFF, 0x10, 0x20, 0x30, 0x40, 0x50);

    munmap((uint64_t *)0xFF, 0x60);    
     
    while(1)  { }

    return 0;
}

