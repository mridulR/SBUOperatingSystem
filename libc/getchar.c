#include <stdio.h>
#include <unistd.h>

int getChar() 
{
  char ch;
  read(0, &ch, 1);
  return ch;
}
