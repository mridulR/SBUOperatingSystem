#include <stdio.h>
#include <string.h>

int strcmp(const char *str1, const char *str2)
{
  unsigned char ch1, ch2;
  
  do  
  {   
    ch1 = (unsigned char) *str1++;
    ch2 = (unsigned char) *str2++;
    if (ch1 == '\0')
    {   
      return ch1 - ch2;
    }   
  }while (ch1 == ch2);
  
  return ch1 -ch2; 
}
