#include <stdio.h>
#include <unistd.h>

#define LINE_LENGTH 1024 

char * gets(char *s)
{
   char ch[1024];
   for (int i = 0; i < 1024; ++i) {
       ch[i] = '\0';
   }

   read(0, ch, 1024);
   
   char *c = ch;
   while(*c != '\0') {
       *s++ = *c++;
   }
   return s;
}
