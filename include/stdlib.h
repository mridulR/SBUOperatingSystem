#ifndef _STDLIB_H
#define _STDLIB_H

#include <sys/types.h>

int main(int argc, char *argv[], char *envp[]);
void exit();

void *malloc(int size);
void free(void *ptr);

#endif
