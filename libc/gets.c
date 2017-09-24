#include <stdio.h>
#include <unistd.h>

#define LINE_LENGTH 1024 

char * gets(char *s)
{
  int rd = 0;
  do
  {
      rd = getchar();
  }while(rd == ' ' || rd == '\t');

  if(rd == '\n')
  {
    *s= '\0';
    return NULL;
  }

  int count = 1;
  do
  {
    *s++ = rd;
    rd = getchar();
    ++count;
  }while(rd != '\n' && rd != -1 && count <= LINE_LENGTH);

  *s = '\0';

  return s;
}
