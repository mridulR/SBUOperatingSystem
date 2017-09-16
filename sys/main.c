#include <sys/types.h>
#include <sys/gdt.h>
#include <sys/kprintf.h>
#include <sys/tarfs.h>
#include <sys/ahci.h>
#include <sys/idt.h>

#define true 1
#define INITIAL_STACK_SIZE 4096

extern Idtr s_Idtr;

uint8_t initial_stack[INITIAL_STACK_SIZE]__attribute__((aligned(16)));
uint32_t* loader_stack;
extern char kernmem, physbase;

void start(uint32_t *modulep, void *physbase, void *physfree)
{
  struct smap_t {
    uint64_t base, length;
    uint32_t type;
  }__attribute__((packed)) *smap;
  while(modulep[0] != 0x9001) modulep += modulep[1]+2;
  for(smap = (struct smap_t*)(modulep+2); smap < (struct smap_t*)((char*)modulep+modulep[1]+2*4); ++smap) {
    if (smap->type == 1 /* memory */ && smap->length != 0) {
      // TODO: uncomment below line
      //kprintf("Available Physical Memory [%p-%p]\n", smap->base, smap->base + smap->length);
    }
  }
  // TODO: uncomment below line
  //kprintf("physfree %p\n", (uint64_t)physfree);
  //kprintf("tarfs in [%p:%p]\n", &_binary_tarfs_start, &_binary_tarfs_end);
  test_interrupt_zero();
  while(true) {
  }
}

void boot(void)
{
  // note: function changes rsp, local stack variables can't be practically used
  // TODO: uncomment below line
  //register char *temp1, *temp2;
  register char *temp2;

  for(temp2 = (char*)0xb8001; temp2 < (char*)0xb8000+160*25; temp2 += 2) *temp2 = 7 /* white */;
  __asm__ volatile (
    "cli;"
    "movq %%rsp, %0;"
    "movq %1, %%rsp;"
    :"=g"(loader_stack)
    :"r"(&initial_stack[INITIAL_STACK_SIZE])
  );
  init_gdt();
  
  // Disable the interrupts
  disable_Interrupts();

  // Initialize IDT and load the Idtr
  init_Idt();
  
  // Enable the interrupts
  enable_Interrupts();

  start(
    (uint32_t*)((char*)(uint64_t)loader_stack[3] + (uint64_t)&kernmem - (uint64_t)&physbase),
    (uint64_t*)&physbase,
    (uint64_t*)(uint64_t)loader_stack[4]
  );
  // TODO: uncomment below line
  /*for(
    temp1 = "!!!!! start() returned !!!!!", temp2 = (char*)0xb8000;
    *temp1;
    temp1 += 1, temp2 += 2
  ) *temp2 = *temp1;*/
  while(1) __asm__ volatile ("hlt");
}
