#ifndef _KSTRING_H
#define _KSTRING_H 

#include<sys/types.h>

void k_concat(char * str1, char * str2);

int kstrcmp(const char * s1, const char * s2);

int kstrlen(const char * str);

char * kstr_tok(char * str, const char delimeter, char ** remaining);

#endif
