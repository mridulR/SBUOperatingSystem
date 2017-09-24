#include <stdlib.h>

void _start(void) {
  // call main() and exit() here
  char *envs[] = {"PATH=/bin/\0"};
  char *args[] = {"Makefile"};
  main(1, args, envs);
  exit();
}
