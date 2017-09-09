#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int cat(int read_fd, int write_fd)
{
    char buffer[4096];
    int bytes;
    memset(buffer, '\0', sizeof(buffer));
    while ((bytes = read(read_fd, buffer, sizeof(buffer))) > 0) {
        if (write(write_fd, buffer, bytes) != bytes) {
            return -1;
        }
        memset(buffer, '\0', sizeof(buffer));
    }
    return (bytes < 0) ? -1 : 0;
}


int main(int argc, char **argv) {
if (argc == 2)
{
        int fd = open(argv[1], O_RDONLY);
        if (fd < 0) {
          fputs("Failed to open file for reading\n", stdout);
        } else {
            if (cat(fd, 1) != 0) {
                fputs("Failed to write file to standard output\n", stdout);
            }
            close(fd);
        }
    }
 return 1;
}
