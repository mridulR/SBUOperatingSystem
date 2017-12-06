#include <stdio.h>
#include <stdlib.h>

#define __NR_read_64 0
#define __NR_write_64 1
#define __NR_getpid_64 39
#define __NR_mmap_64 9 
#define __NR_munmap_64 11 

int main(int argc, char *argv[], char *envp[]) {
    

    /*char buf[1024];
    char write_buff[1024];
    write_buff[0] = 'a';
    write_buff[1] = 'b';
    write_buff[2] = 'c';
    write_buff[3] = 'd';
    write_buff[4] = '\0';

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

	// Read sys call

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
     
	// write sys call
    __asm__ __volatile__
    (
        "movq %1,%%r8\n"
        "movq %2,%%r9\n"
        "movq %3,%%r10\n"
        "movq %4,%%r11\n"
        "int $0x80\n"
        "movq %%rax,%0\n"
        : "=r" (write_ret)
        : "g"(write_syscall_num), "g"(arg1), "g"(arg2_write), "g"(arg3_write)
        : "r8", "r9", "r10", "r11"
    );

    } */

/*

	// Get pid
	uint64_t get_pid_syscall_num = (uint64_t)__NR_getpid_64;
	uint64_t pid = 0;
	__asm__ __volatile__
      (
		"movq %1,%%r8\n"
		"int $0x80\n"
		"movq %%rax,%0\n"
		: "=r" (pid)
		: "g"(get_pid_syscall_num)
		:
	);

*/

	// mmap testing
	
	uint64_t get_mmap_syscall_num = (uint64_t)__NR_mmap_64;
	uint64_t arg1 = 0;
	uint64_t arg2 = 0;
	uint64_t arg3 = 0;
	uint64_t arg4 = 0;
	uint64_t arg5 = 0;
	uint64_t arg6 = 0;

	uint64_t ret_val = 0;

	__asm__ __volatile__
	(
	"movq %1,%%r8\n"
	"movq %2,%%r9\n"
	"movq %3,%%r10\n"
	"movq %4,%%r11\n"
	"movq %5,%%r12\n"
	"movq %6,%%r13\n"
	"movq %7,%%r14\n"
	"int $0x80\n"
	"movq %%rax,%0\n"
	: "=r" (ret_val)
	: "g"(get_mmap_syscall_num) , "g"(arg1), "g"(arg2), "g"(arg3), "g"(arg4), "g"(arg5), "g"(arg6)
	: "r8", "r9", "r10", "r11"
	);



/*	// munmap testing
	uint64_t get_munmap_syscall_num = (uint64_t)__NR_munmap_64;

	uint64_t arg1 = 0;
	uint64_t arg2 = 0;
	uint64_t arg3 = 0;
	uint64_t arg4 = 0;
	uint64_t arg5 = 0;
	uint64_t arg6 = 0;
     
	uint64_t ret_val = 0;


    __asm__ __volatile__
     (
     "movq %1,%%r8\n"
     "movq %2,%%r9\n"
     "movq %3,%%r10\n"
     "movq %4,%%r11\n"
     "movq %5,%%r12\n"
     "movq %6,%%r13\n"
	 "movq %7,%%r14\n"
     "int $0x80\n"
     "movq %%rax,%0\n"
     : "=r" (ret_val)
     : "g"(get_munmap_syscall_num) , "g"(arg1), "g"(arg2), "g"(arg3), "g"(arg4), "g"(arg5), "g"(arg6)
     : "r8", "r9", "r10", "r11" 
     );

*/

    
     
    while(1)  { }
    return 0;
}

