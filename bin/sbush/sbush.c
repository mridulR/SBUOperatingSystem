#include <stdio.h>
#include <stdlib.h>

int global_val = 1;

int main(int argc, char *argv[], char *envp[]) {
    int local = 3;
    local = global_val + 1;
    global_val = local + 1;
    while(1)  { }
    return 0;
}

