#ifndef _STDLIB_H
#define _STDLIB_H

#include <sys/defs.h>

char *getenv(const char *name);
int setenv(const char *name, const char *value, int overwrite);

int main(int argc, char *argv[], char *envp[]);
void exit(int status);

void *malloc(size_t size);
void free(void *ptr);

#endif
