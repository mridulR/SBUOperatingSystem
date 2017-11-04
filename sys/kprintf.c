#include <sys/kprintf.h>
#include <sys/types.h>
#include <stdarg.h>
#include <sys/memcpy.h>
#include <sys/memset.h>

#define MAX_SCREEN_WIDTH 160
#define MAX_SCREEN_HEIGHT 24


void HandleString(char* str, char* currAddr);
void HandleSignedInt(const int i_argVal, char* currAddr);
void HandleAddress(uint64_t addr, char* currAddr);
void HandleUnsignedInt(unsigned int u_argVal, char* currAddr);
void HandleNewLine();
void HandleChar(char c, char* currAddr);
void UpdateHeightWidth(char* currAddr);
void HandleEverythingElse(char fmt, char* currAddr);

char *getCurrentAddress() {
  if (current_height > 23) {
    // do scrolling
    char * baseAddr = (char *) VIDEO_BUFFER_BASE_ADDR;
    for (int i = 0; i < MAX_SCREEN_HEIGHT - 1; i++) {
      for (int j = 0; j < MAX_SCREEN_WIDTH; j++) {
        baseAddr[i * MAX_SCREEN_WIDTH + j] = baseAddr[(i + 1) * MAX_SCREEN_WIDTH + j];
      }
    }
    current_height = 23;
    current_width = 0;
    for (int index = 0; index < MAX_SCREEN_WIDTH; index++) {
      if (index % 2 == 0) {
        baseAddr[23 * MAX_SCREEN_WIDTH + index] = '\0';
        baseAddr[23 * MAX_SCREEN_WIDTH + index + 1] = 0x07;
      }
    }    
  }
  char *currAddr = (char *)(VIDEO_BUFFER_BASE_ADDR);
  currAddr = currAddr + (current_height * MAX_SCREEN_WIDTH + current_width);
  return currAddr;
}

void UpdateHeightWidth(char * currAddr) {
  uint64_t currAddValue = (uint64_t) currAddr; 
  currAddValue -= (uint64_t)VIDEO_BUFFER_BASE_ADDR;
  current_height = currAddValue / MAX_SCREEN_WIDTH;
  current_width =  currAddValue % MAX_SCREEN_WIDTH;
}

void printTime(unsigned int time) {
  char *currAddr = TIME_ADDRESS;
  unsigned int num = time;
  const unsigned int zeroHex = 0x30;
  char ch[100] = {0};
  int numdigits = 0;

  while(num > 0) {
    ch[numdigits++] = num % 10;
    num=num/10;
  }
  for(int i = numdigits-1; i >= 0; --i) {
      *currAddr++ = (zeroHex + ch[i]);
      *currAddr++ = 0x07;
  }
  *currAddr++ = 's';
  *currAddr++ = 0x07;
}

void printKeypress(char keypress, char * address) {
  char *temp = address;
  *temp++ = keypress;
  *temp++ = 0x07;
}

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
              HandleString(str, getCurrentAddress());
              break;
          case 'd': 
              i_argVal = va_arg(apList, int);
              HandleSignedInt(i_argVal, getCurrentAddress());
              break;
          case 'x': 
              u_argVal = va_arg(apList, unsigned int);
              HandleUnsignedInt(u_argVal, getCurrentAddress());
              break;
          case 'c':
              c = (char) va_arg(apList, int);
              HandleChar(c, getCurrentAddress());
              break;
          case 'p':
              addr = va_arg(apList, uint64_t);
              HandleAddress(addr, getCurrentAddress());
              break;
        }
       fmt++; // For escaping the alphabets in %c, %p etc
      } else {
          HandleEverythingElse(*fmt, getCurrentAddress());
          fmt++;
      }
    }
    va_end(apList);
}

void HandleEverythingElse(char fmt, char* currAddr) {
   if (fmt == '\n') {
     HandleNewLine();
   } else {
     *currAddr++ = fmt;
     *currAddr++ = 0x07;
     UpdateHeightWidth(currAddr);
   }
}

void HandleChar(char ch, char* currAddr) {
  *currAddr++ = ch;
  *currAddr++ = 0x07;
  UpdateHeightWidth(currAddr);
}

void HandleNewLine() {
  current_height++;
  current_width = 0;
  getCurrentAddress(); // For scrolling if required
}

void HandleString(char* str, char* currAddr) {
  uint64_t availableSpace = (uint64_t)(VIDEO_BUFFER_MAX_LIMIT) - (uint64_t)currAddr;
  char saveBufferLines[160 * 10];
  while(*str != 0 && availableSpace != 0){
      *currAddr++ = *str++;
      *currAddr++ = 0x07;
      availableSpace -=2;
      if(availableSpace == 0){
        const char *startAddress = (char *)VIDEO_BUFFER_BASE_ADDR + (160 * 15);
        uint64_t size = (uint64_t)currAddr - (uint64_t)startAddress;
        memset(saveBufferLines,'\0', 160 *10);
        memcpy(saveBufferLines, startAddress, size);
        memset((char *)VIDEO_BUFFER_BASE_ADDR,'\0', 160 * 24);
        memcpy((char *)VIDEO_BUFFER_BASE_ADDR, saveBufferLines, size);
        currAddr = (char *)VIDEO_BUFFER_BASE_ADDR + size;
      }
  }
  UpdateHeightWidth(currAddr);
}

void HandleSignedInt(const int i_argVal, char* currAddr) {
  int num = i_argVal;
  const int zeroHex = 0x30;
  char ch[100];
  int numdigits = 0;
  if(num == 0) {
    *currAddr++ = '0';
    *currAddr++ = 0x07;
    UpdateHeightWidth(currAddr);
    return;
  }
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
  UpdateHeightWidth(currAddr);
}

void HandleAddress(uint64_t addr, char* currAddr){

  char ch[100] = {48}; 
  uint64_t num = addr;
  int i = 0, rem = 0;

  *currAddr++ = '0';
  *currAddr++ = 0x07;
  *currAddr++ = 'x';
  *currAddr++ = 0x07;

  if(num == 0) {
    *currAddr++ = '0';
    *currAddr++ = 0x07;
    UpdateHeightWidth(currAddr);
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
 for (int j = i-1; j >= 0; j--) {
    //printf("%c", ch[j]);
    *currAddr++ = ch[j];
    *currAddr++ = 0x07;
  }
  UpdateHeightWidth(currAddr);
}


void HandleUnsignedInt(unsigned int u_argVal, char* currAddr) {

  unsigned int num = u_argVal;
  const unsigned int zeroHex = 0x30;
  char ch[100] = {0};
  int numdigits = 0;
  if(num == 0) {
    *currAddr++ = '0';
    *currAddr++ = 0x07;
    UpdateHeightWidth(currAddr);
    return;
  }
  while(num > 0) {
    ch[numdigits++] = num % 10; 
    num=num/10;
  }
  for(int i = numdigits-1; i >= 0; --i) {
      *currAddr++ = (zeroHex + ch[i]);
      *currAddr++ = 0x07;
  }
  UpdateHeightWidth(currAddr);
}
