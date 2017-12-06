#include <sys/idt.h>
#include <sys/kprintf.h>
#include <sys/memset.h>
#include <sys/types.h>
#include <sys/asm_util.h>
#include <sys/page_table.h>

#define __NR_write_64 1

/* RPL    BIT 0, 1     Requested Privilege Level. The CPU checks these bits before any selector is
                       changed. Also system calls can be executed in userspace (ring 3, see this) 
                       without misfeasance using the ARPL (Adjust Requested Privilege Level) instruction.
   TI     BIT 2        Table index; 0 = GDT, 1 = LDT
   Index  BIT 3..15    Index to a Descriptor of the table. 
*/
#define IDT_SELECTOR 0x8

//32-bit Interrupt gate: 0x8E ( P=1, DPL=00, S=0, type=1110 => type_attr=1000_1110=0x8E)
#define INTERRUPT_GATE_TYPE_ATTR 0x8E
#define SYSCALL_GATE_TYPE_ATTR   0xEE

// Initilizing the IDT structure
static Idtr __attribute__((used)) s_Idtr = {0};

Idtd s_Idtd[MAX_NUM_INTERRUPTS] = {{0}};

//static int fault_count = 0;

void enable_Interrupts() {
  //Disable ISR's first
  __asm__ __volatile__
  (
    "sti\n"
    "ret\n"
  );
}

void disable_Interrupts() {
  //Disable ISR's first
  __asm__ __volatile__
  (
    "cli"
  );
  return;  
}

extern void pit_interrupt_service_routine();
extern void keyboard_interrupt_service_routine();
extern void helper_general_permission_fault_handler();

void general_permission_fault() {
    kprintf("\nGENERAL PERMISSION FAULT !!!");
    uint64_t ret = readCR2();
    uint64_t error = 0;
    __asm__ __volatile__ 
    ( "movq %%rsp, %0\n"
      :"=r"(error)
      :
    );
    kprintf("\nError Code: (%p, %d)", error, *(uint64_t *)(error+8));
    kprintf("\nCR2 Value: %p", ret);
    while(1) {}
}

void general_page_fault() {
    kprintf("\nGENERAL PAGE FAULT !!!");
    uint64_t ret = readCR2();
    uint64_t error = 0;
    __asm__ __volatile__ 
    ( "movq %%rsp, %0\n"
      "cli\n"
      :"=r"(error)
      :
    );
    kprintf("\nError Code: (%p, %d)", error, *(uint64_t *)(error+8));
    kprintf("\nCR2 Value: %p", ret);
    page_fault_handler(ret);
    //flush_tlb_entry(ret);
    __asm__ __volatile__ ("movq %%cr3,%%rax\n" : : );
    __asm__ __volatile__ ("movq %%rax,%%cr3\n" : : );
    __asm__ __volatile__ ("sti\n" : : );
 
    //while(1) {}
}

void helper_interrupt_service_routine() {
  kprintf("\nDEFAULT INTERRUPT HANDLER CALLED: ");
  while(1) {}
}

void default_interrupt_service_routine();

void helper_page_fault_handler();

void load_idt() {
  __asm__ __volatile__
  (
      "lidt (s_Idtr)\n"
  );
}

void syscall_handler();

void helper_syscall_handler() {
    uint64_t syscallNum = 0;
    uint64_t arg2 = 0;
    __asm__ __volatile__ 
    (  "movq %%rsi, %0\n"
       "movq %%rdi, %1\n"
       :"=r"(syscallNum), "=r"(arg2)
       :
    );

    *(char *)arg2 = 'a';
    kprintf(" Invoked syscall handler !!! SyscallNum = %p Arg2= %p *arg2 = %c ", syscallNum, arg2, *(char*)arg2);
    //while(1) {}
    return;
}

// Initializes IDT and IDTR
void init_Idt() {
  
  // The caller must disable ISR's first before invoking this and enable after init_Idt()
  // Initialize the idt structure
  // TODO: Currently we install the same IH for all. Change this later after basic things work.
  s_Idtr.limit = (sizeof(Idtd) * MAX_NUM_INTERRUPTS - 1);
  s_Idtr.base = (uint64_t)&s_Idtd[0];
  
  kprintf("s_idtr: lim (%d) base(%p)\n", s_Idtr.limit, s_Idtr.base);

  memset(&s_Idtd[0], 0, sizeof(Idtd) * MAX_NUM_INTERRUPTS - 1);

  for(int i = 0; i< MAX_NUM_INTERRUPTS; ++i){
      set_interrupt_service_routine(i, INTERRUPT_GATE_TYPE_ATTR, default_interrupt_service_routine);
  }
  
  set_interrupt_service_routine(32, INTERRUPT_GATE_TYPE_ATTR, pit_interrupt_service_routine);
  set_interrupt_service_routine(33, INTERRUPT_GATE_TYPE_ATTR, keyboard_interrupt_service_routine);
  set_interrupt_service_routine(13, INTERRUPT_GATE_TYPE_ATTR, helper_general_permission_fault_handler);
  set_interrupt_service_routine(14, INTERRUPT_GATE_TYPE_ATTR, helper_page_fault_handler);

  set_interrupt_service_routine(127, SYSCALL_GATE_TYPE_ATTR, syscall_handler);
  set_interrupt_service_routine(128, SYSCALL_GATE_TYPE_ATTR, syscall_handler);

  //s_Idtd[127].type = 0xEE;
  //s_Idtd[128].type = 0xEE;
  // Load the IDT
  //load_idt();
   __asm__ __volatile__
  (
      "lidt (s_Idtr)\n"
  );

}

void set_interrupt_service_routine(uint32_t interrupt_service_num, uint8_t type, InterruptHandler interrupt_service_routine) {
  
  if(!interrupt_service_routine || interrupt_service_num > MAX_NUM_INTERRUPTS || interrupt_service_num < 0) {
    return;
  }

  uint64_t addr = (uint64_t)interrupt_service_routine;

  s_Idtd[interrupt_service_num].offset_1 = (uint16_t)(addr & 0xffff); 
  s_Idtd[interrupt_service_num].selector = (uint16_t)IDT_SELECTOR;
  s_Idtd[interrupt_service_num].ist = 0;
  s_Idtd[interrupt_service_num].type = (uint8_t)type;
  s_Idtd[interrupt_service_num].offset_2 = (uint16_t)((addr >> 16) & 0xffff);
  s_Idtd[interrupt_service_num].offset_3 = (uint32_t)((addr >> 32) & 0xffffffff);

  s_Idtd[interrupt_service_num].zero = 0;
}


Idtd* get_interrupt_service_routine(uint32_t interrupt_service_num) {
  if (interrupt_service_num < MAX_NUM_INTERRUPTS){
    return &s_Idtd[interrupt_service_num];
  }

  return 0;
}


void test_interrupt_zero() {
  __asm__ __volatile__
  (
    "int $32\n"
  );
}
