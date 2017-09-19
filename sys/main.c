#include <sys/types.h>
#include <sys/gdt.h>
#include <sys/kprintf.h>
#include <sys/tarfs.h>
#include <sys/ahci.h>
#include <sys/idt.h>
#include <sys/pic.h>
#include <sys/pit.h>

#define true 1
#define INITIAL_STACK_SIZE 4096

extern Idtr s_Idtr;
uint8_t initial_stack[INITIAL_STACK_SIZE]__attribute__((aligned(16)));
uint32_t* loader_stack;
extern char kernmem, physbase;


uint64_t current_width = 0;
uint64_t current_height = 0;
char *TIME_ADDRESS = (char *)(BASE_ADDR + ((160 * 24) + 120));

void start(uint32_t *modulep, void *physbase, void *physfree)
{

  struct smap_t {
    uint64_t base, length;
    uint32_t type;
  }__attribute__((packed)) *smap;
  while(modulep[0] != 0x9001) modulep += modulep[1]+2;
  for(smap = (struct smap_t*)(modulep+2); smap < (struct smap_t*)((char*)modulep+modulep[1]+2*4); ++smap) {
    if (smap->type == 1   && smap->length != 0) {
     // kprintf("Available Physical Memory [%p-%p]\n", smap->base, smap->base + smap->length);
    }
  }
  kprintf("physfree %p\n", (uint64_t)physfree);
  kprintf("tarfs in [%p:%p]\n", &_binary_tarfs_start, &_binary_tarfs_end);

  test_interrupt_zero();
  printTime("TimeTillBoot\0");

  for(int i = 0; i< 50; i++) {
      kprintf("%d\n", i);
  } 
  while(1) {
  }
}

void boot(void)
{
  // note: function changes rsp, local stack variables can't be practically used
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
  
  // Initialize IDT and load the Idtr
  init_Idt();

  // Initialize the PIC 
  init_pic(PROT_MODE_MASTER_PIC_VECTOR_OFFSET, PROT_MODE_SLAVE_PIC_VECTOR_OFFSET);

  // Initialize the PIT
  init_pit();
  //TODO: Think more on outportb ? What's the best place to end interrupt ?
  //outportb(0x20, 0x20);
  // Start the timer 
  enable_Interrupts();
  
  start(
    (uint32_t*)((char*)(uint64_t)loader_stack[3] + (uint64_t)&kernmem - (uint64_t)&physbase),
    (uint64_t*)&physbase,
    (uint64_t*)(uint64_t)loader_stack[4]
  );
  while(1) __asm__ __volatile__ ("hlt");  
}
