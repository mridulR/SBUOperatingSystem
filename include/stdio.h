#ifndef _STDIO_H
#define _STDIO_H

#include <sys/defs.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2

static const int EOF = -1;

int putchar(int c);
int puts(const char *s);
int printf(const char *format, ...);

void print(char *s);

char *gets(char *s);

int fputs(const char *s, FILE *stream);

char *gets(char *s);

int getChar(void);

#endif
