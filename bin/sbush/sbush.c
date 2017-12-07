#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>

/*#define __NR_read_64 0
#define __NR_write_64 1
#define __NR_getpid_64 39
#define __NR_mmap_64 9 
#define __NR_munmap_64 11 
*/

int main(int argc, char *argv[], char *envp[]) {
    

    char buf[1024];
    uint64_t arg1 = 0;
    uint64_t arg2 = (uint64_t)&buf;
    uint64_t arg3 = 1024;
    char ch[10];
    ch[0] = 's';
	ch[1] = 'b';
	ch[2] = 'u';
	ch[3] = 's';
	ch[4] = 'h';
	ch[5] = '~';
	ch[6] = '>';
	ch[7] = ' ';
    ch[8] = '\0';
    
    while(1) {
	  write(arg1,(void *)&ch, 7);
	  read(arg1, (void *)arg2, arg3);
	  write(arg1, (void *)arg2, 20);
	  //getpid();
    }

/*	// mmap testing
	
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
*/


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

