#include <stdio.h>
#include <string.h>
#include <unistd.h>

int fputs(const char * str, FILE * stream) {
 write(stream->handle, str, strlen(str));
 return 0;
}

