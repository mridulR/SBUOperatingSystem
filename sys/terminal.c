#include <sys/terminal.h>
#include <sys/memcpy.h>
#include <sys/memset.h>
#include <sys/kprintf.h>

char terminal_buffer[4096];
uint64_t buffer_length = 0;

volatile bool is_flushed = false;


struct terminal_operation_pntrs terminal_operations = {
    terminal_read,
    terminal_write
};


int64_t terminal_read(int fd, void * buf, uint64_t count) {
    is_flushed = false;
    memset(terminal_buffer, '\0', buffer_length);
    bool is_read_done = is_flushed;
    while (!is_read_done) {
        // wait till \n is pressed
        is_read_done = is_flushed;
    }
    memcpy(buf, terminal_buffer, buffer_length);
    memset(terminal_buffer, '\0', buffer_length);
    count = buffer_length;
    buffer_length = 0;
    return count;
}

int64_t terminal_write(int fd, void * buf, uint64_t count) {
    ((char *) (buf))[count] = '\0';
    kprintf("%s", buf);
    return count;
}

void terminal_enqueue(char ch) {
    if ((int)(ch) == 0) {
       return; // This is key release
    }
    kprintf("%c", ch);
    if (ch == '\b') {
        terminal_buffer[buffer_length] = '\0';
        buffer_length = buffer_length - 1;
        terminal_buffer[buffer_length] = '\0';
        return;
    }
    if (ch == '\n') {
        terminal_buffer[buffer_length] = '\0';
        kprintf("\n Enter was pressed\n");
        is_flushed = true;
        return;
    }
    if (buffer_length == 0 && ch == '\b') {
        // do nothing
        return;
    }
    terminal_buffer[buffer_length] = ch;
    buffer_length = buffer_length + 1;
}







