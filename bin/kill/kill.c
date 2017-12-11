#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>

int32_t atoi(char *ch) {
    int32_t num = 0;
    while (*ch != '\0') {
        num = num * 10 + (*ch - '0');
        ch++;
    }
    return num;
}


int main(int argc, char *argv[], char *envp[]) {
    if( argv[1] != NULL && argv[2] != NULL){
        if(*argv[1] == '-') {
            int flag = atoi(argv[1] + 1);
            int pid = atoi(argv[2]);
            kill(flag, pid);
        }
    }
    return 0;
}

