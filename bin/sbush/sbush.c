#include <stdio.h>
#include <stdlib.h>

#define __NR_read_64 0
#define __NR_write_64 1


int main(int argc, char *argv[], char *envp[]) {
    
    // Read sys cal testing

    char buf[1024];
    char write_buff[1024];
    /*write_buff[0] = 'a';
    write_buff[1] = 'b';
    write_buff[2] = 'c';
    write_buff[3] = 'd';
    write_buff[4] = '\0';*/

    uint64_t read_ret = 0;
    uint64_t write_ret = 0;
    uint64_t read_syscall_num = (uint64_t)__NR_read_64;
    uint64_t write_syscall_num = (uint64_t)__NR_write_64;
    uint64_t arg1 = 0;
    uint64_t arg2 = (uint64_t)&buf;
    uint64_t arg2_write = (uint64_t) &write_buff;
    uint64_t arg3 = 1024;
    uint64_t arg3_write = 4;

    while (1) {

    __asm__ __volatile__
    (
        "movq %1,%%r8\n"
        "movq %2,%%r9\n"
        "movq %3,%%r10\n"
        "movq %4,%%r11\n"
        "int $0x80\n"
        "movq %%rax,%0\n"
        : "=r" (read_ret)
        : "g"(read_syscall_num) , "g"(arg1), "g"(arg2), "g"(arg3)
        : "r8", "r9", "r10", "r11"
    );

	}
     
    while(1)  { }
    return 0;
}

