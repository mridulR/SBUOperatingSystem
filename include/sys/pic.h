#ifndef _PIC_H
#define _PIC_H

#include <sys/types.h>

typedef void (*IRQHandler) (void);

#define MASTER_PIC_ID 0
#define SLAVE_PIC_ID  1 

//! Master PIC IRQ's
//! IRQ 2 is for cascaded IRQ's from slave PIC
#define PIC_IRQ_TIMER              0
#define PIC_IRQ_KEYBOARD           1 
#define PIC_IRQ_SERIAL2            3 
#define PIC_IRQ_SERIAL1            4 
#define PIC_IRQ_PARALLEL2          5 
#define PIC_IRQ_FLOPPY_CONTROLLER  6 
#define PIC_IRQ_PARALLEL1          7 

//! Slave PIC IRQ's
//! IRQ 9 will be send to master PIC
#define PIC_IRQ_REAL_TIME_CLOCK 8 
#define PIC_IRQ_MOUSE           12 
#define PIC_IRQ_CP_FP_IPR       13 
#define PIC_IRQ_PRIMARY_ATA     14 
#define PIC_IRQ_SECONDARY_ATA   15 

//! REAL Mode Configurations
#define REAL_MODE_MASTER_PIC_VECTOR_OFFSET 0x08
#define REAL_MODE_SLAVE_PIC_VECTOR_OFFSET  0x70

//! Protected Mode Configurations 
//! TODO: Check if required or not? Osdev say's setting correct offset on
//! changing modes
#define PROT_MODE_MASTER_PIC_VECTOR_OFFSET 0x20
#define PROT_MODE_SLAVE_PIC_VECTOR_OFFSET  0x28

//! Please refer in future to see the bits mapping from Osdev
//! Reference Link: http://wiki.osdev.org/PIC
//! Master PIC port configurations
#define MASTER_PIC_BASE_ADDR 0x20
#define MASTER_PIC_COMMAND   0x20
#define MASTER_PIC_DATA      0x21

//! Slave PIC port configurations
#define SLAVE_PIC_BASE_ADDR 0xA0
#define SLAVE_PIC_COMMAND   0xA0
#define SLAVE_PIC_DATA      0xA1

//! PIC Command Word 1 bit masks
#define PIC_ICW1_MASK_ICW4          0x01
#define PIC_ICW1_MASK_SINGLE        0x02
#define PIC_ICW1_MASK_INTERVAL4     0x04
#define PIC_ICW1_MASK_LEVEL         0x08
#define PIC_ICW1_MASK_INIT          0x10

//! PIC Command Word 4 bit masks
#define PIC_ICW4_MASK_8086          0x01
#define PIC_ICW4_MASK_AUTO_EOI      0x02
#define PIC_ICW4_MASK_BUF_MASTER    0x08
#define PIC_ICW4_MASK_BUF_SLAVE     0x0C
#define PIC_ICW4_MASK_SFNM          0x10

//! ISR - In-Service Register 
//!       Stores the current serving IRQ number
//! IRR - Interrupt Request Register
//!       Stores the next IRQ number
#define PIC_OCW3_ISR   0x0a
#define PIC_OCW3_IRR   0x0b

//! EOI command bitset for PIC
#define PIC_EOI        0x20

//! Receives a 8/16/32-bit value from an I/O location
uint8_t inportb(uint16_t port);

//! Sends a 8/16/32-bit value on a I/O location.  
void outportb(uint16_t port, uint8_t val);

//! Forces the CPU to wait for an I/O operation to complete.
void io_wait(void);

//! write the data to the pic outportb
void write_data_to_pic(uint8_t data, uint8_t picNum);

//! write the command to the pic outportb
void write_command_word_to_pic(uint8_t cmd, uint8_t picNum);

//! read the data from the pic inportb 
uint8_t read_data_from_pic(uint8_t picNum);

//! read the command from the pic inportb 
uint8_t read_command_word_from_pic(uint8_t picNum);

//! Initializes the pic with init command and data 
void init_pic(uint8_t offset1, uint8_t offset2);

//! get the combined value of in-service requests of both master and slave pics  
//! first 8 bits  - Master pic
//! second 8 bits - Slave pic
uint16_t get_pic_isr();

//! get the combined value of next interrupt request of both master and slave pics  
//! first 8 bits  - Master pic
//! second 8 bits - Slave pic
uint16_t get_pic_irr();

//! For writing the EOI command to PIC 
void end_of_interrupt(uint8_t interruptNum);

//! Gets the interrupt_service_routine from the IDT for the irq
IRQHandler get_ir_from_idt(uint16_t interrupt_num);

//! Sets the interrupt_service_routine in the IDT
void set_ir_in_idt(uint16_t interrupt_num, IRQHandler interrupt_service_routine);

#endif
