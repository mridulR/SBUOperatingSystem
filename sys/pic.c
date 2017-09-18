#include <sys/pic.h>
#include <sys/idt.h>

//32-bit Interrupt gate: 0x8E ( P=1, DPL=00, S=0, type=1110 => type_attr=1000_1110=0x8E)
#define INTERRUPT_GATE_TYPE_ATTR 0x8E


uint8_t inportb(uint16_t port) {
  uint8_t ret;
  __asm__ __volatile__ 
  ( 
    "inb %1, %0\n"
    :"=a"(ret)
    :"Nd"(port) 
  );
  return ret;
}

void outportb(uint16_t port, uint8_t val)
{
  __asm__ __volatile__ 
  ( 
    "outb %0, %1\n" 
    : 
    : "a"(val), "Nd"(port)
  );
}

// TODO: Can we change from al to %0 ? Will check while testing the code. Also
// check 0x80 implications later.
void io_wait(void)
{
  __asm__ __volatile__ 
  (
    "outb %%al, $0x80"
    : 
    :"a"(0) 
  );
}

void write_data_to_pic(uint8_t data, uint8_t picNum) {

  uint8_t portId = 0; 

  if(picNum == MASTER_PIC_ID) {
    portId = MASTER_PIC_DATA;
  }
  else if (picNum == SLAVE_PIC_ID) {
    portId = SLAVE_PIC_DATA;
  }
  else {
    return;
  }
  outportb(portId, data);
}


void write_command_word_to_pic(uint8_t cmd, uint8_t picNum) {

  uint8_t portId = 0; 

  if(picNum == MASTER_PIC_ID) {
    portId = MASTER_PIC_COMMAND;
  }
  else if (picNum == SLAVE_PIC_ID) {
    portId = SLAVE_PIC_COMMAND;
  }
  else {
    return;
  }
  outportb(portId, cmd);
}

uint8_t read_data_from_pic(uint8_t picNum) {
   
  uint8_t portId = 0; 

  if(picNum == MASTER_PIC_ID) {
    portId = MASTER_PIC_DATA;
  }
  else if (picNum == SLAVE_PIC_ID) {
    portId = SLAVE_PIC_DATA;
  }
  else {
    return 0;
  }
  return inportb(portId);
}

uint8_t read_command_word_from_pic(uint8_t picNum) {
   
  uint8_t portId = 0; 

  if(picNum == MASTER_PIC_ID) {
    portId = MASTER_PIC_COMMAND;
  }
  else if (picNum == SLAVE_PIC_ID) {
    portId = SLAVE_PIC_COMMAND;
  }
  else {
    return 0;
  }
  return inportb(portId);
}
/* Protected Mode: Send offset1 = 0x20, offset2 = 0x28 for resettign the PIC's.

   Protected Mode Mapping:
   IRQ 0 - 7  => 0x20 to 0x27
   IRQ 8 - 15 => 0x28 to 0x2F

   Real Mode: Send offset1 = 0x08, offset2 = 0x70 for resettign the PIC's.
   Real Mode Mapping:
   IRQ 0 - 7  => 0x08 to 0x0F
   IRQ 8 - 15 => 0x70 to 0x77

   @ Use appropriate base addresses to work in real/protected mode
     **NOTE : Assumed that caller disables the hardware interrupt**
*/
void init_pic(uint8_t offset1, uint8_t offset2) {

  // Interrupts are already disabled by caller.
  // See main.c Line No: 48 "instruction 'cli'

  uint8_t master_pic_data  = read_data_from_pic(MASTER_PIC_ID);
  uint8_t slave_pic_data   = read_data_from_pic(SLAVE_PIC_ID);
  
  write_command_word_to_pic( PIC_ICW1_MASK_INIT | PIC_ICW1_MASK_ICW4, MASTER_PIC_ID);
  io_wait();
  write_command_word_to_pic( PIC_ICW1_MASK_INIT | PIC_ICW1_MASK_ICW4, SLAVE_PIC_ID);
  io_wait();
  
  // setting the correct vector offset for pics
  // NOTE: IRQ 0-7 map to 0x0F and they are reserved by intel
  write_data_to_pic(offset1, MASTER_PIC_ID);
  io_wait();
  write_data_to_pic(offset2, SLAVE_PIC_ID);
  io_wait();

  // connect master IRQ2 to slave Pic
  write_data_to_pic(0x04, MASTER_PIC_ID);
  io_wait();
  write_data_to_pic(0x02, SLAVE_PIC_ID);
  io_wait();

  write_data_to_pic(PIC_ICW4_MASK_8086, MASTER_PIC_ID);
  io_wait();
  write_data_to_pic(PIC_ICW4_MASK_8086, SLAVE_PIC_ID);
  io_wait();
  
  // set the initial data in both pic
  write_data_to_pic(master_pic_data, MASTER_PIC_ID);
  io_wait();
  write_data_to_pic(slave_pic_data, SLAVE_PIC_ID);
  io_wait();
}


uint16_t get_pic_isr() {

  uint16_t ret = 0;
  write_command_word_to_pic(PIC_OCW3_ISR, MASTER_PIC_ID);
  write_command_word_to_pic(PIC_OCW3_ISR, SLAVE_PIC_ID);
  ret =  ((uint16_t)read_command_word_from_pic(MASTER_PIC_ID) << 8) | read_command_word_from_pic(SLAVE_PIC_ID);
  return ret;
}

uint16_t get_pic_irr() {

  uint16_t ret = 0;
  write_command_word_to_pic(PIC_OCW3_IRR, MASTER_PIC_ID);
  write_command_word_to_pic(PIC_OCW3_IRR, SLAVE_PIC_ID);
  ret =  ((uint16_t)read_command_word_from_pic(MASTER_PIC_ID) << 8) | read_command_word_from_pic(SLAVE_PIC_ID);
  return ret;
}

void end_of_interrupt(uint8_t interruptNum) {
  if(interruptNum > 15) {
    return;
  }
  else if(interruptNum >= 8) {
  // We must send end of interrupt request to slave only when req num is equal
  // or more than 8
    write_command_word_to_pic(PIC_EOI, SLAVE_PIC_ID);
  }
  // We must always send end of interrupt request to master
  write_command_word_to_pic(PIC_EOI, MASTER_PIC_ID);
}


IRQHandler get_ir_from_idt(uint16_t interrupt_num) {
  if(interrupt_num >= 256) {
    return 0;
  }

  Idtd* idt = get_interrupt_service_routine(interrupt_num);
  if(!idt) {
    return 0;  
  }
  uint64_t addr = 0;
  uint16_t offset1 = idt->offset_1;
  uint16_t offset2 = idt->offset_2;
  uint32_t offset3 = idt->offset_3;
  addr = ((uint64_t)offset3 << 32)| ((uint32_t)offset2 << 16) | offset1;
  return (IRQHandler)addr;
}

void set_ir_in_idt(uint16_t interrupt_num, IRQHandler interrupt_service_routine) {
  if(interrupt_num >= 256) {
    return;
  }
  set_interrupt_service_routine(interrupt_num, INTERRUPT_GATE_TYPE_ATTR, interrupt_service_routine);
}

