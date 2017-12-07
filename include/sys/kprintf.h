#ifndef __KPRINTF_H
#define __KPRINTF_H
#include <sys/types.h>

#define VIDEO_BUFFER_BASE_ADDR      0xFFFFFFFF800b8000
#define VIDEO_BUFFER_BASE_PHYS_ADDR 0xb8000
#define VIDEO_BUFFER_MAX_LIMIT  0xb8f00

extern char* TIME_ADDRESS;
extern char* KEYPRESS_ADDRESS;
extern char* CTRL_KEYPRESS_ADDRESS;
extern uint64_t current_width, current_height;

void kprintf(const char *fmt, ...);

void printTime(unsigned int time);
void printKeypress(char keypress, char * address);

void clrscreen();

#endif
