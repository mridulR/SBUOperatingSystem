#ifndef _TERMINAL_HEADER_H
#define _TERMINAL_HEADER_H

#include <sys/types.h>

#define true 1
#define false 0
    
typedef uint8_t bool;


struct terminal_operation_pntrs {
    int64_t (*terminal_read)(int fd, void * buf, uint64_t count);
    int64_t (*terminal_write)(int fd, void * buf, uint64_t count);
};

int64_t terminal_read(int fd, void * buf, uint64_t count);
int64_t terminal_write(int fd, void * buf, uint64_t count);

void terminal_enqueue(char ch);

extern struct terminal_operation_pntrs terminal_operations;

#endif
