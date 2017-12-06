#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>

int main(int argc, char *argv[], char *envp[]) {
    int a = 40;
    //int b = 30;
    char ch;
    __asm__ __volatile__
    ( 
        "movq %0, %%rsi\n"
        "movq %1, %%rdi\n"
        "int $0x80\n" 
        :
        :"r"((uint64_t)a), "r"((uint64_t)&ch)
    );

    /*__asm__ __volatile__
    ( 
        "movq %0, %%rsi\n"
        "movq %1, %%rdi\n"
        "int $0x80\n" 
        :
        :"r"((uint64_t)ch), "r"((uint64_t)ch)
    );*/
    //char ch = 'A';
    //putchar(ch);
    while(1)  { }
    return 0;
}

