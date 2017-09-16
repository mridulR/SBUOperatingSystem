#include <sys/kprintf.h>
#include <sys/types.h>
#include <stdarg.h>

char* HandleString(char* str, char* currAddr);
char* HandleSignedInt(const int i_argVal, char* currAddr);
char* HandleAddress(uint64_t addr, char* currAddr);
char* HandleUnsignedInt(unsigned int u_argVal, char* currAddr);

void kprintf(const char *fmt, ...)
{

    char c, *str;
    int i_argVal;
    unsigned int u_argVal;
    uint64_t addr;
    va_list apList;

    va_start(apList, fmt);
    while (*fmt) {
      if(*fmt == '%') {
        fmt++;
        switch (*fmt) {
          case 's':
              str = va_arg(apList, char *);
              currAddr = HandleString(str, currAddr);
              break;
          case 'd': 
              i_argVal = va_arg(apList, int);
              currAddr = HandleSignedInt(i_argVal, currAddr);
              break;
          case 'x': 
              u_argVal = va_arg(apList, unsigned int);
              currAddr = HandleUnsignedInt(u_argVal, currAddr);
              break;
          case 'c':
              c = (char) va_arg(apList, int);
              //printf("%c", c);
              *currAddr++ = c;
              *currAddr++ = 0x07;
              break;
          case 'p':
              addr = va_arg(apList, uint64_t);
              //TODO: Debug this please.
              currAddr = HandleAddress(addr, currAddr);
              break;
        }
      }
      if(*fmt!='%'){
        *currAddr++ = *fmt++;
        *currAddr++ = 0x07;
      }
    }
    va_end(apList);
}

char* HandleString(char* str, char* currAddr) {
  while(*str != 0){
      //printf("%c", *str++);
      *currAddr++ = *str++;
      *currAddr++ = 0x07;
  }
  return currAddr;
}

char* HandleSignedInt(const int i_argVal, char* currAddr) {
  int num = i_argVal;
  const int zeroHex = 0x30;
  char ch[100];
  int numdigits = 0;

  if(i_argVal < 0){
      //printf("%c", 0x2d);
      *currAddr++ = 0x2d;
      *currAddr++ = 0x07;
      num = num * (-1);
  }
  while(num > 0) {
    ch[numdigits++] = num % 10; 
    num=num/10;
  }
  for(int i = numdigits-1; i >= 0; --i) {
      //printf("%c", (zeroHex + ch[i]));
      *currAddr++ = zeroHex + ch[i];
      *currAddr++ = 0x07;
  }
  return currAddr;
}

char* HandleAddress(uint64_t addr, char* currAddr){

  char ch[100]= {0}; 
  uint64_t num = addr;
  int i = 0, rem = 0;

  while(num > 0) {
     rem = num % 16; 
    if (rem < 10) {
      ch[i++] = 48 + rem;
    }
    else {
      ch[i++] = 55 + rem;
    }
    num=num/16;
  }

  for (int j = i; j >= 0; j--) {
    //printf("%c", ch[j]);
    *currAddr++ = ch[j];
    *currAddr++ = 0x07;
  }
  return currAddr;
}


char* HandleUnsignedInt(unsigned int u_argVal, char* currAddr) {

  unsigned int num = u_argVal;
  const unsigned int zeroHex = 0x30;
  char ch[100] = {0};
  int numdigits = 0;

  while(num > 0) {
    ch[numdigits++] = num % 10; 
    num=num/10;
  }
  for(int i = numdigits-1; i >= 0; --i) {
      //printf("%c", (zeroHex + ch[i]));
      *currAddr++ = (zeroHex + ch[i]);
      *currAddr++ = 0x07;
  }
  return currAddr;
}
