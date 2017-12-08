#include <stdio.h>

void *memset(void *s, int c, int n)
{
    unsigned char *ch = s;
    for(int i=0; i < n; ++i)
    {
        *ch++ = c;
    }
    return s;
}
