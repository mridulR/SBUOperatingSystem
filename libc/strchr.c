#include <stdio.h>
#include <string.h>

char *strchr(char *s, char c)
{
  char *ch = s;
  while(*ch!= '\0' && *ch!='\n')
  {
    if(*ch++ == c){
      return (ch-1);
    } 
  }
  return NULL;
}
