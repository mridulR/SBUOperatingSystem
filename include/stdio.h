#ifndef _STDIO_H
#define _STDIO_H

#include <sys/types.h>

extern FILE *STDERR;
extern FILE *STDIN;
extern FILE *STDOUT;

#define stdin  STDIN
#define stdout STDOUT
#define stderr STDERR

int putchar(int c);
int puts(const char *s);
int printf(const char *format, ...);

void print(char *s);

char *gets(char *s);

int fputs(const char *s, FILE *stream);

char *fgets(char *s, int size, FILE *stream);

int fgetc(FILE *fp);

char *gets(char *s);

int getchar(void);

#endif
