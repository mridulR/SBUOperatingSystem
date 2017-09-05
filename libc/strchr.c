#include <stdio.h>
#include <string.h>


char *strchr(const char *str, int chr)
{
    while (*str != (char)chr)
        if (!*str++)
            return 0;
    return (char *)str;
}


