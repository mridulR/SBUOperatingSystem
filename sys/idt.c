#include <sys/idt.h>
#include <sys/kprintf.h>
#include <sys/types.h>
#include <sys/asm_util.h>

/* RPL    BIT 0, 1     Requested Privilege Level. The CPU checks these bits before any selector is
                       changed. Also system calls can be executed in userspace (ring 3, see this) 
                       without misfeasance using the ARPL (Adjust Requested Privilege Level) instruction.
   TI     BIT 2        Table index; 0 = GDT, 1 = LDT
   Index  BIT 3..15    Index to a Descriptor of the table. 
*/
#define IDT_SELECTOR 0x8

//32-bit Interrupt gate: 0x8E ( P=1, DPL=00, S=0, type=1110 => type_attr=1000_1110=0x8E)
#define INTERRUPT_GATE_TYPE_ATTR 0x8E

void *memset(void *s, int c, int n)
{
    unsigned char *ch = s;
    for(int i=0; i < n; ++i)
    {
        *ch++ = c;
    }
    return s;
}

// Initilizing the IDT structure
static Idtr __attribute__((used)) s_Idtr = {0};

Idtd s_Idtd[MAX_NUM_INTERRUPTS] = {{0}};

void enable_Interrupts() {
  //Disable ISR's first
  __asm__ __volatile__
  (
    "sti"
  );
  return;  
}

void disable_Interrupts() {
  //Disable ISR's first
  __asm__ __volatile__
  (
    "cli"
  );
  return;  
}

void helper_interrupt_service_routine() {
  kprintf("Hello World !!!");
}
void default_interrupt_service_routine();

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
  
  // Load the IDT
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
