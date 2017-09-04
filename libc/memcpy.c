#include <string.h>

void *memcpy(void *dest, const void *src, size_t n)
{
    const unsigned char *source = src;
    unsigned char *destination = dest;

    if(source == NULL || destination == NULL)
    {
      return dest;
    }

    for(signed int i = 0; i<(signed int)n; ++i)
    {
      *destination++ = *source++;
    }
    return destination;
}

