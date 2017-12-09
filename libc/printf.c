#include <stdio.h>
#include <stdarg.h>
#include <sys/defs.h>
#include <unistd.h>

void HandleString(char* str);
void HandleSignedInt(const int i_argVal);
void HandleAddress(uint64_t addr);
void HandleUnsignedInt(unsigned int u_argVal);
void HandleNewLine();
void HandleChar(char c );
void HandleEverythingElse(char fmt);

int printf(const char *fmt, ...) {
    uint64_t count = 0;
    char c, *str;
    int i_argVal;
    unsigned int u_argVal;
    uint64_t addr;
    va_list apList;

    va_start(apList, fmt);
    while (*fmt) {
      ++count;
      if(*fmt == '%') {
        fmt++;
        switch (*fmt) {
          case 's':
              str = va_arg(apList, char *);
              HandleString(str);
              break;
          case 'd': 
              i_argVal = va_arg(apList, uint64_t);
              HandleSignedInt(i_argVal);
              break;
          case 'u': 
              i_argVal = va_arg(apList, uint64_t);
              HandleUnsignedInt(i_argVal);
              break;
          case 'x': 
              u_argVal = va_arg(apList, uint64_t);
              HandleAddress(u_argVal);
              break;
          case 'c':
              c = (char) va_arg(apList, int);
              HandleChar(c);
              break;
          case 'p':
              addr = va_arg(apList, uint64_t);
              HandleAddress(addr);
              break;
        }
       fmt++; // For escaping the alphabets in %c, %p etc
      } else {
          HandleEverythingElse(*fmt);
          fmt++;
      }
    }
    va_end(apList);
    return count;
}

void HandleEverythingElse(char fmt) {
   if (fmt == '\n') {
     HandleNewLine();
   } else {
       write(1, &fmt, 1);
   }
}

void HandleChar(char ch) {
  if (ch == '\n') {
     HandleNewLine();
     return;
  }
  write(1, &ch, 1);
}

void HandleNewLine() {
  char ch[2];
  ch[0] = '\n';
  ch[1] = '\0';
  write(1, &ch, 1);
}

int slen(char *s) {
    if(s == NULL) {
        return 0;
    }
    int count = 0;
    while(*s != '\0') {
        ++count;
        s++;
    }
    return count;
}

void HandleString(char* str) {
  int len = slen(str);
  write(1, str, len);
}

void HandleSignedInt(const int i_argVal) {
  int num = i_argVal;
  char ch[100];
  int numdigits = 0;
  if(num == 0) {
    char val = '0';
    write(1, &val, 1);
    return;
  }
  if(i_argVal < 0){
      char val = 0x2d;
      write(1, &val, 1);
      num = num * (-1);
  }
  while(num > 0) {
    ch[numdigits++] = 48 + (num % 10); 
    num=num/10;
  }
  for(int i = numdigits-1; i >= 0; --i) {
      write(1, &ch[i], 1);
  }
}

void HandleAddress(uint64_t addr){

  char ch[100] = {48}; 
  uint64_t num = addr;
  int i = 0, rem = 0;
  
  char val = '0';
  write(1, &val, 1);
  val = 'x';
  write(1, &val, 1);

  if(num == 0) {
    val = '0';
    write(1, &val, 1);
    return;
  }

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
 for (int j = i+1; j >= 0; j--) {
    write(1, &ch[j], 1);
  }
}


void HandleUnsignedInt(unsigned int u_argVal) {
  unsigned int num = u_argVal;
  char ch[100] = {0};
  int numdigits = 0;
  char val;
  if(num == 0) {
    val = '0';
    write(1, &val, 1);
    return;
  }
  while(num > 0) {
    ch[numdigits++] = 48 + (num % 10); 
    num=num/10;
  }
  for(int i = numdigits-1; i >= 0; --i) {
      write(1, &ch[i], 1);
  }
}

