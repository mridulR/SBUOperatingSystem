#ifndef __KPRINTF_H
#define __KPRINTF_H
#include <sys/types.h>

#define BASE_ADDR 0xb8000

extern char* TIME_ADDRESS;
extern char* KEYPRESS_ADDRESS;
extern char* CTRL_KEYPRESS_ADDRESS;
extern uint64_t current_width, current_height;

void kprintf(const char *fmt, ...);

void printTime(unsigned int time);
void printKeypress(char keypress, char * address);

#endif
