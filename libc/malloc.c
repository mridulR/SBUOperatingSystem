#include <unistd.h>
#include <sys/types.h>

#define MAP_ANONYMOUS 0x20
#define MAP_PRIVATE 0x02
#define MAP_SHARED 0x01
#define PROT_READ 0x1
#define PROT_WRITE 0x2
#define PROT_EXEC 0x4
#define PROT_NONE 0x0

typedef struct {
  void *addr;
  int  blockSize;
  struct meta_block * next;
} meta_block;

meta_block *ROOT = NULL;

meta_block * getNewBlock(int len) {
   //Aloocate the info block
   meta_block* new_meta_block = 
        mmap(NULL, sizeof(meta_block)+len, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0); 

   if (new_meta_block == NULL) {
     //Failed to allocate
     return NULL;
   }
  
  new_meta_block->blockSize  = len;
  new_meta_block->addr = new_meta_block + sizeof(meta_block);
  new_meta_block->next = NULL;

  return new_meta_block;
}


void allocateMemoryWhenListIsEmpty(meta_block ** root, size_t size) {
  *root = getNewBlock(size);
}

meta_block * getMemoryBlockFromOs(size_t size) {
 meta_block * new_metaBlock = getNewBlock(size);
 new_metaBlock->next = ROOT->next;
 ROOT = new_metaBlock;
 return ROOT;
}

void *malloc(int size) {
  if (size <= 0) {  // Invalid Arguments
    return NULL;
  }
  if (ROOT == NULL) {   // List is empty
    allocateMemoryWhenListIsEmpty(&ROOT, size);
    return (ROOT->addr);
  }
  meta_block * last_block = getMemoryBlockFromOs(size);

  return (last_block);
}

void free (void * ptr) {
  if (ptr == NULL) {
    // do nothing
    return;
  }

  meta_block* trav = ROOT;
  meta_block* prev = NULL;
  while (trav != NULL) {
    if(trav->addr == ptr){
      if(prev != NULL){
        prev->next = trav->next;
      }
      munmap(trav, trav->blockSize+ sizeof(meta_block));
      return;
    }
    prev = trav;
    trav = (meta_block *)trav->next;
  }
  return;
}
