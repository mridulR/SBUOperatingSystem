#ifndef __KPRINTF_H
#define __KPRINTF_H

#define BASE_ADDR 0xb8000

extern char *currAddr;

void kprintf(const char *fmt, ...);

#endif
