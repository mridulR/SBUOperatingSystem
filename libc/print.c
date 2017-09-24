#include <stdio.h>

void print(char *s)
{
  if(!s)
    return;
  while(*s != '\n' && *s !='\0')
  {
    putchar(*s);
    ++s;
  }
  return;
}
