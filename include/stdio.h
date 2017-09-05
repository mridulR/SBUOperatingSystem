#ifndef _STDIO_H
#define _STDIO_H

#include <sys/defs.h>

int putchar(int c);
int puts(const char *s);
int printf(const char *format, ...);

void print(char *s);

int getchar();

char *gets(char *s);

int fputs(const char *s, FILE *stream);

char *fgets(char *s, int size, FILE *stream);


#endif
