#include <stdio.h>

void *memcpy(void *d, const void *s, int n)
{
    const unsigned char *src  = s;
    unsigned char *dest = d;
    for(int i=0; i < n; ++i)
    {
        *dest++ = *src++;
    }
    return dest;
}
