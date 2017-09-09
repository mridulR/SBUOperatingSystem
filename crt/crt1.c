#include <stdlib.h>

void _start(void) {
  // call main() and exit() here
  char *envs[] = {"PATH=/bin/\0"};
  main(1, NULL, envs);
  exit();
}
