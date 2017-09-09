#include <stdio.h>
//#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/syscall.h>

#define O_RDONLY    0x0000
#define O_WRONLY    0x0001
#define O_RDWR      0x0002
#define O_ACCMODE   0x0003

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


int main(int argc, char *argv[], char *env[]) {

if (argc >= 1)
{
  fputs("aaya", stdout);
  fputs(argv[0], stdout);
  fputs(argv[1], stdout);

  int fd = open(argv[0], O_RDONLY);
  if (fd < 0) {
    fputs("Failed to open file for reading\n", stdout);
  } else {
      if (cat(fd, 1) != 0) {
          fputs("Failed to write file to standard output\n", stdout);
      }
      close(fd);
  }
}
else{
fputs("H", stdout);
  cat(0,1);
}
 return 1;
}

