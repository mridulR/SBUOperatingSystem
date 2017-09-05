#include <stdio.h>
#include <unistd.h>

char *fgets(char *s, int size, FILE *stream) {
  read(stream->handle, s, size);
  return s;
}
