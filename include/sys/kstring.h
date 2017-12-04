#ifndef _KSTRING_H
#define _KSTRING_H 

#include<sys/types.h>

int kstrcmp(const char * s1, const char * s2);

int kstrlen(char * str);

char * kstr_tok(char * str, const char delimeter, char ** remaining);

#endif
