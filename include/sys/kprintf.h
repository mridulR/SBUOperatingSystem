#ifndef __KPRINTF_H
#define __KPRINTF_H
#include <sys/types.h>

#define BASE_ADDR 0xb8000

extern char* TIME_ADDRESS;
extern uint64_t current_width, current_height;

void kprintf(const char *fmt, ...);

void printTime(const char *fmt); // end it with /0

#endif
