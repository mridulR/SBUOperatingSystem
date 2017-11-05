#include <sys/phys_pages.h>
#include <sys/page_table.h>
#include <sys/test/test.h>

#define PHYS_HIGH_BASE_ADDR    0x100000
#define PHYS_HIGH_LIMIT_ADDR   0x6066000
#define PHYS_LOW_BASE_ADDR     0x0
#define PHYS_LOW_LIMIT_ADDR    0x9F000

#define PHYS_HIGH_PAGE_COUNT (((uint64_t)PHYS_HIGH_LIMIT_ADDR - (uint64_t)PHYS_HIGH_BASE_ADDR)/PHYS_PAGE_SIZE)
#define PHYS_LOW_PAGE_COUNT  (((uint64_t)PHYS_LOW_LIMIT_ADDR  - (uint64_t)PHYS_LOW_BASE_ADDR)/PHYS_PAGE_SIZE)
#define PHYS_PAGE_COUNT (PHYS_HIGH_PAGE_COUNT + PHYS_LOW_PAGE_COUNT)

#define PHYS_PAGE_SIZE 4096
#define PAGE_SIZE  0x1000

Phys_page s_phys_page[PHYS_HIGH_PAGE_COUNT]= {NULL};

uint64_t s_free_page_count;
uint64_t s_max_page_count;
uint64_t s_cur_page_index;
uint64_t s_phys_base_addr;
uint64_t s_phys_limit_addr;
uint64_t s_kern_start;
uint64_t s_kern_end;
extern char kernmem;

void init_phys_page(uint32_t *modulep, uint64_t kern_start, uint64_t kern_end) {
  uint64_t max_phys_mem_slot = 0;
  s_kern_start = kern_start;
  s_kern_end = kern_end;

  struct smap_t {
    uint64_t base, length;
    uint32_t type;
  }__attribute__((packed)) *smap;
  while(modulep[0] != 0x9001) modulep += modulep[1]+2;
  for(smap = (struct smap_t*)(modulep+2); smap < (struct
      smap_t*)((char*)modulep+modulep[1]+2*4); ++smap) {
    if (smap->type == 1   && smap->length != 0) {
      kprintf("Available Physical Memory [%p-%p] Type %p \n", smap->base,
              smap->base + smap->length, smap->type);
      if(smap->length > max_phys_mem_slot) {
          s_phys_base_addr = smap->base;
          s_phys_limit_addr = smap->base + smap->length;
          max_phys_mem_slot = smap->length;
      }
    }
  }

  s_cur_page_index = 0;
  s_free_page_count = 0;

  uint64_t start = kern_end;
  s_phys_base_addr = kern_end;
  uint64_t end = s_phys_limit_addr;
  kprintf("Start: %p End: %p", start, end);
  while(start < end) {
      start = start + (uint64_t)PAGE_SIZE;
      if(start >= kern_start && start <= kern_end) {
        continue;
      }
      s_phys_page[s_free_page_count++].next = (Phys_page *)start; 

      s_max_page_count = s_free_page_count;
  }
  s_phys_page[s_free_page_count].next = NULL;
  
  //test_physical_pages(kern_start, kern_end);
  //test_allocate_deallocate_page();
  init_kernel_page_table(kern_start, kern_end);
  return; 
}

uint64_t get_phys_addr(uint64_t index) {
    return (uint64_t)s_phys_base_addr + (uint64_t)(index * PAGE_SIZE);
}

uint64_t get_first_free_page() {
    if(s_free_page_count == 0) {
         kprintf("KERNEL PANIC: Out of Memory !!!");
         return 0;
    }
    uint64_t cur_addr = (uint64_t)s_phys_base_addr + (uint64_t)((s_cur_page_index) * PAGE_SIZE);
    uint64_t next_addr = (uint64_t)s_phys_page[s_cur_page_index].next;
    s_cur_page_index = s_cur_page_index + (next_addr - cur_addr)/ PAGE_SIZE;
    s_free_page_count--;
    return cur_addr;
}

uint64_t allocate_phys_page() {
    return get_first_free_page();
}

void deallocate_phys_page(uint64_t addr) {

    if(addr <= s_phys_base_addr || addr >= s_phys_limit_addr || (addr >=
       s_kern_start && addr <= s_kern_end)) {
         kprintf("KERNEL PANIC: Physical Address out of range or kernel range !!!");
         return;
    }
    uint64_t pageIndex = ((addr - s_phys_base_addr) / PAGE_SIZE);
    if(pageIndex > s_max_page_count) {
        kprintf("KERNEL PANIC: Physical address out of range !!!");
        return;
    }
    uint64_t savePrevAddr = get_phys_addr(s_cur_page_index);    
    s_cur_page_index = pageIndex;
    s_phys_page[s_cur_page_index].next = (Phys_page *)savePrevAddr;
    return;
}

