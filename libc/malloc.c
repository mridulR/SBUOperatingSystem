#include <unistd.h>
#include <sys/types.h>

#define MAP_ANONYMOUS 0x20
#define MAP_PRIVATE 0x02
#define MAP_SHARED 0x01
#define PROT_READ 0x1
#define PROT_WRITE 0x2
#define PROT_EXEC 0x4
#define PROT_NONE 0x0

void *malloc(int size) {
  if (size <= 0) {  
    // Invalid Arguments
    return NULL;
  }
  uint64_t* addr = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0); 
  return (void *)addr;
}

void free (void * ptr) {
  if (ptr == NULL) {
    // do nothing
    return;
  }
  munmap(ptr, 0);
}
