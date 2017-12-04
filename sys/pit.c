#include <sys/pit.h>
#include <sys/pic.h>
#include <sys/kprintf.h>
#include <sys/asm_util.h>
#include <sys/idt.h>

//! Just for referencing in future.
//! Bookmark the link - https://en.wikibooks.org/wiki/X86_Assembly/Programmable_Interval_Timer

#define FREQUENCY (100)
#define MAX_FREQUENCY (1193181)

// TODO: Similarly add other bits with mask and or them to generate command.
// Currently, using 0x36 => 00 11 011 0 ( channel, acess mode, operating mode, binary)
#define PIT_SQUARE_WAVE_COMMAND_VALUE 0x34

//! extern all the pic utility functions which PIT will use
extern void set_ir_in_idt(uint16_t interrupt_num, IRQHandler interrupt_service_routine);
extern void outportb(uint16_t port, uint8_t val);

void pit_interrupt_service_routine();

static uint32_t g_pit_count = 0;
static uint64_t time_in_sec = 1;

uint32_t get_pit_count() {
  return g_pit_count;
}

void helper_calculate_timer() {
  ++g_pit_count; 
  if(g_pit_count % 4 == 0) {
    ++time_in_sec;
    printTime(time_in_sec/100);
  }
  outportb(0x20, 0x20);
}

void init_pit() {

  set_ir_in_idt(32, pit_interrupt_service_routine);
  //load_idt();

  uint16_t base_frequency = 40000;
  uint64_t max_frequency = 1193181;
 
  // Calculate the pit frequency
  uint16_t frequency = (uint16_t)(max_frequency/base_frequency);
 
  outportb(PIT_COMMAND, PIT_SQUARE_WAVE_COMMAND_VALUE);
  outportb(PIT_CHANNEL_0, (uint8_t)frequency);
  outportb(PIT_CHANNEL_0, (uint8_t)(frequency >> 8));
  g_pit_count = 0; 
  //outportb(0x20, 0x20);
  /*__asm__ __volatile__ ("sti\n");*/
}

