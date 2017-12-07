#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>

int main(int argc, char *argv[], char *envp[]) {
    
	/*
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


	mmap(0, 0, 0, 0, 0, 0);

	*/
    munmap(0, 0);	

     
    while(1)  { }
    return 0;
}

