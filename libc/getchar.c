#include <stdio.h>
#include <unistd.h>

int getchar()
{
  char ch;
  int rd = read(0, &ch, 1);
  if(rd == -1)
    return -1;

  return ch;
}
