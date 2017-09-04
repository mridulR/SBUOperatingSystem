#include <string.h>

void *memset(void *s, int c, size_t n)
{
    unsigned  char *ch = s;
    if(ch == NULL)
    {
        return s;
    }

    for(signed int i=0; i < (signed int) n; ++i)
    {
        *ch++ = c;
    }
    return s;
}
