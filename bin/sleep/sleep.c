#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <dirent.h>

int32_t atoi(char *ch) {
    int32_t num = 0;
    while (*ch != '\0') {
        num = num * 10 + (*ch - '0');
        ch++;
    }
    return num;
}

int main(int argc, char *argv[], char *envp[]) {
    if(argv[1] != NULL) {
        int time = atoi(argv[1]);
        sleep(time);
    }
    else {
        printf(" No argument for sleep !");
    }
    return 0;
}

