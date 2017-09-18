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
  kprintf("The emergence and growth of blogs in the late 1990s coincided with the advent of web publishing tools that facilitated the posting of content by non-technical users who did not have much experience with HTML or computer programming. Previously, a knowledge of such technologies as HTML and File Transfer Protocol had been required to publish content on the Web, and as such, early Web users tended to be hackers and computer enthusiasts. In the 2010s, the majority are interactive Web 2.0 websites, allowing visitors to leave online comments, and it is this interactivity that distinguishes them from other static websites.[2] In that sense, blogging can be seen as a form of social networking service. Indeed, bloggers do not only produce content to post on their blogs, but also often build social relations with their readers and other bloggers.[3] However, there are high-readership blogs which do not allow comments.Many blogs provide commentary on a particular subject or topic, ranging from politics to sports. Others function as more personal online diaries, and others function more as online brand advertising of a particular individual or company. A typical blog combines text, digital images, and links to other blogs, web pages, and other media related to its topic. The ability of readers to leave publicly viewable comments, and interact with other commenters, is an important contribution to the popularity of many blogs. However, blog owners or authors often moderate and filter online comments to remove hate speech or other offensive content. Most blogs are primarily textual, although some focus on art (art blogs), photographs (photoblogs), videos, music (MP3 blogs), and audio (podcasts). In education, blogs can be used as instructional resources. These blogs are referred to as edublogs. Microblogging is another type of blogging, featuring very short posts.On 16 February 2011, there were over 156 million public blogs in existence. On 20 February 2014, there were around 172 million Tumblr[4] and 75.8 million WordPress[5] blogs in existence worldwide.");

  kprintf("Hello world!!! \n");
  kprintf("My age is %d\n", 19);

  printTime("TimeTillBoot\0");

  for(int i = 0; i< 50; i++) {
      kprintf("%d\n", i);
  } 
  //kprintf("physfree %p\n", (uint64_t)physfree);
  //kprintf("tarfs in [%p:%p]\n", &_binary_tarfs_start, &_binary_tarfs_end);
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
  
  // Enable the interrupts
  enable_Interrupts();

  start(
    (uint32_t*)((char*)(uint64_t)loader_stack[3] + (uint64_t)&kernmem - (uint64_t)&physbase),
    (uint64_t*)&physbase,
    (uint64_t*)(uint64_t)loader_stack[4]
  );
  while(1) __asm__ __volatile__ ("hlt");  
}
