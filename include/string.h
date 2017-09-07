#ifndef _STRING_H
#define _STRING_H

#include <sys/defs.h>



int strcmp(const char * s1, const char * s2);

/**
   @brief calculate the length of the string terminated by NULL or '\0'

 */
int strlen(const char *str);

/**
   @brief Fill the buffer pointing by address s by value c upto n bytes  

 */
void *memset(void *s, int c, size_t n);

/**
   @brief Copy 'n' bytes from source address to destination address.

 */
void *memcpy(void *dest, const void *src, size_t n);

#endif
