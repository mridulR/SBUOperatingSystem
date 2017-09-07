#include <unistd.h>
#include <sys/types.h>
#include <string.h>


typedef struct {
  size_t sizeOfBlock;
  int free;
  struct meta_block * next;
} meta_block;

meta_block *ROOT = NULL;

meta_block * getNewBlock(size_t size) {
meta_block * new_meta_block = sbrk(0);
   if (new_meta_block == NULL) {
     // sbrek failed so return
     return NULL;
   }

   void * new_combined_memory_block = sbrk(size + (size_t) sizeof(meta_block));
  if (new_combined_memory_block == NULL) {
    // sbrek failed so return
    return NULL;
  }

  new_meta_block->sizeOfBlock = size;
  new_meta_block->free = 0;
  new_meta_block->next = NULL;

  return new_meta_block;
}


void allocateMemoryWhenListIsEmpty(meta_block ** root, size_t size) {
  *root = getNewBlock(size);
}

meta_block * getMemoryOfRequisiteSizeFromList(size_t size) {
  meta_block* trav = ROOT;
  while (trav != NULL) {
    if (trav->sizeOfBlock >= size && trav->free == 1) {
       trav->free = 0;
       return trav;
    }
    trav = (meta_block *)trav->next;
  }
  return NULL;
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
    return (ROOT + 1);
  }

  meta_block * requisiteBlock = getMemoryOfRequisiteSizeFromList(size);

  if (requisiteBlock != NULL) {
    return (requisiteBlock + 1);
  }
 
  meta_block * last_block = getMemoryBlockFromOs(size);

 return (last_block + 1);
}

void free (void * ptr) {
  if (ptr == NULL) {
    // do nothing
    return;
  }
  meta_block * casted_meta_block = (meta_block *) ptr - 1;
  if (casted_meta_block != NULL) {
    casted_meta_block->free = 1;
  }
}
