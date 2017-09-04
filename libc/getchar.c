#include <stdio.h>
#include <unistd.h>

int getChar(void)
{
  char ch;
  int  rd = read(0, &ch, 1);
  return ch;
}


