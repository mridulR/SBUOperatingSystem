#include <stdio.h>
#include <string.h>

char *strchr(char *s, char c)
{
  char *ch = s;
  if(!s)
    return NULL;

  while(ch!=NULL)
  {
    if(*ch++ == c){
      return (ch-1);
    } 
  }
  return NULL;
}
