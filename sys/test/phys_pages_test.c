#include <sys/phys_pages.h>
#include <sys/types.h>
#include <sys/test/test.h>

#define true 1
#define false 0

#define PHYS_HIGH_BASE_ADDR    0x100000
#define PHYS_HIGH_LIMIT_ADDR   0x6066000
#define PHYS_LOW_BASE_ADDR     0x0
#define PHYS_LOW_LIMIT_ADDR    0x9F000

#define PHYS_HIGH_PAGE_COUNT (((uint64_t)PHYS_HIGH_LIMIT_ADDR - (uint64_t)PHYS_HIGH_BASE_ADDR)/PHYS_PAGE_SIZE)
#define PHYS_LOW_PAGE_COUNT  (((uint64_t)PHYS_LOW_LIMIT_ADDR  - (uint64_t)PHYS_LOW_BASE_ADDR)/PHYS_PAGE_SIZE)
#define PHYS_PAGE_COUNT (PHYS_HIGH_PAGE_COUNT + PHYS_LOW_PAGE_COUNT)

#define PHYS_PAGE_SIZE 4096
#define PAGE_SIZE  0x1000


typedef uint8_t bool;
typedef struct phys_page Phys_page;

extern Phys_page s_phys_page[];
extern uint64_t s_free_page_count;
extern uint64_t s_max_page_count;
extern uint64_t s_cur_page_index;
extern uint64_t s_phys_base_addr;
extern uint64_t s_phys_limit_addr;
extern uint64_t s_kern_start;
extern uint64_t s_kern_end;
extern char kernmem;

void test_physical_pages(uint64_t kern_start, uint64_t kern_end) {
  int count = 0;
  bool error = false;
  uint64_t addr;
  while(count < s_max_page_count) {
      if((uint64_t)s_phys_page[count].next >= (uint64_t)kern_start &&
         (uint64_t)s_phys_page[count].next <= (uint64_t)kern_end) {
        kprintf("TEST: Mapped a kernel page in Free list !!!!  %p \n",
                (uint64_t)s_phys_page[count].next);
        if(!error) {
            addr = (uint64_t)s_phys_page[count].next;
        }
        error = true;
      }
      count++;
  }
  if(!error) {
      kprintf("TEST: KERNEL RANGE [%p  %p] NOT IN FREE LIST \n",
              (uint64_t)kern_start, (uint64_t)kern_end);
  }
  else{
        kprintf("TEST: KERNEL PAGE mapped in Free list !!!! Start Address\
                Overlap =  %p \n", addr);
  }
  kprintf("TEST: FREE LIST RANGE [%p  %p] \nPAGE_COUNT %d QEMU_PAGE_COUNT %d\n",
          (uint64_t)s_phys_base_addr,
          (uint64_t)s_phys_page[s_max_page_count-1].next, s_max_page_count,
          (uint64_t)PHYS_HIGH_PAGE_COUNT);

  kprintf("TEST: ABOVE FREE LIST RANGE IS NULL [(%d):%p  (%d):%p (%d):%p] \n",
          s_max_page_count, (uint64_t)s_phys_page[s_max_page_count].next,
          s_max_page_count+1, (uint64_t)s_phys_page[s_max_page_count+1].next,
          PHYS_HIGH_PAGE_COUNT-1,
          (uint64_t)s_phys_page[PHYS_HIGH_PAGE_COUNT-1].next);

  if((uint64_t)&kernmem != 0xFFFFFFFF80200000) {
      kprintf("TEST FAIL: kernmem adress changed from 0xFFFFFFFF80200000 to %p", &kernmem);
  }

  return;
}

