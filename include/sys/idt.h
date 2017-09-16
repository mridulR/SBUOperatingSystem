#ifndef _IDT_H
#define _IDT_H

#include <sys/types.h>

#define MAX_NUM_INTERRUPTS 256
#define INTERRUPT_GATE 0x8E
#define TRAP_GATE 0x8F

struct idtd{
    uint16_t offset_1;
    uint16_t selector;
    uint8_t ist; 
    uint8_t type;
    uint16_t offset_2;
    uint32_t offset_3;
    uint32_t zero;
};

typedef struct idtd Idtd;

struct idtr {
    uint16_t limit;
    uint64_t base;
};

typedef struct idtr Idtr;

void init_Idt();

typedef void (*InterruptHandler) (void);

void set_interrupt_service_routine(uint32_t ser_num, uint8_t type, InterruptHandler interrupt_service_routine);

Idtd* get_interrupt_service_routine();

void disable_Interrupts();

void enable_Interrupts();

void test_interrupt_zero();

#endif
