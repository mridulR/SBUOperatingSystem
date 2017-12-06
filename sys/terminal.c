#include <sys/terminal.h>
#include <sys/memcpy.h>
#include <sys/memset.h>
#include <sys/kprintf.h>

char terminal_buffer[4096];
uint64_t buffer_length = 0;

volatile bool is_flushed = false;
extern void enable_Interrupts();
extern void disable_Interrupts();

int64_t terminal_read(int fd, void * buf, uint64_t count) {
    enable_Interrupts();
    kprintf("\nAddrs - %p   %p    %p", fd, (uint64_t) buf, count);
    is_flushed = false;
    memset(terminal_buffer, '\0', 4095);
    memset(buf, '\0', count);
    bool is_read_done = is_flushed;
    kprintf("\n Reading inputs in buffer\n");
    while (!is_read_done) {
        // wait till \n is pressed
        is_read_done = is_flushed;
    }
    kprintf("\nRead input chars in buffer, will copy now\n");
    memcpy(buf, terminal_buffer, buffer_length);
    kprintf("\nCopy done\n");
    ((char *)(buf))[buffer_length] = '\0';
    memset(terminal_buffer, '\0', 4095);
    kprintf("\n buffer has ===> %d  -  %s\n", buffer_length, buf);
    int result = buffer_length;
    buffer_length = 0;
    //disable_Interrupts();
    return result;
}


int64_t terminal_write(int fd, void * buf, uint64_t count) {
    if (count == 0) {
        return 0;
    }
    kprintf("\n write invoked with %d --- %s", count, buf);
    ((char *) (buf))[count] = '\0';
    kprintf("%s", buf);
    return count;
}

struct terminal_operation_pntrs terminal_operations = {
    &terminal_read,
    &terminal_write
};


void terminal_enqueue(char ch) {
    if ((int)(ch) == 0) {
       return; // This is key release
    }
    if (buffer_length >= 4000) {
        kprintf("\n Terminal buffer full. Emptying buffer at this time. Retry with 4000 chars limit \n");
        memset(terminal_buffer, '\0', 4095);
        buffer_length = 0;
        is_flushed = true;
        return;
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







