#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>

void dummy() {
    int a = 40;
    char ch;
    __asm__ __volatile__
    ( 
        "movq %0, %%rsi\n"
        "movq %1, %%rdi\n"
        "int $0x80\n" 
        :
        :"r"((uint64_t)a), "r"((uint64_t)&ch)
    );
    return;
}

int main(int argc, char *argv[], char *envp[]) {
    dummy();
    while(1)  { }
    return 0;
}

