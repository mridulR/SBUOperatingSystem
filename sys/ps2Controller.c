#include <sys/types.h>
#include <sys/ps2Controller.h>
#include <sys/pic.h>
#include <sys/kprintf.h>
#include <sys/asm_util.h>

//! extern all the pic utility functions which PS2 Controller will use 
extern void set_ir_in_idt(uint16_t interrupt_num, IRQHandler interrupt_service_routine);
extern void outportb(uint16_t port, uint8_t val);
extern uint8_t inportb(uint16_t port);
uint8_t uppercase = 0;
uint8_t ctrlKey = 0;

char map[89] = {'\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', '\t', 'q', 
                 'w', 'e', 'r', 't', 'y', 'u', 'i', '0', 'p', '[', ']', '\r', '\0', 'a', 's', 'd', 'f', 'g',
                 'h', 'j', 'k', 'l', ';', '\'', '\0', '\0', '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.',
                 '/', '\0', '*', '\0', ' ', '\0', 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, '\0', '\0',
                 '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.', '\0', '\0', '\0', 122, 123};

void keyboard_interrupt_service_routine();

void helper_keyboard_handler() {
  char ch = inportb(0x60);
  uint8_t key = ch & 0xff; 
  switch (key) {
  case 0x2A:
       uppercase = 1;
       break;
  case 0x36:
       uppercase = 1;
       break;
  case 0xAA:
       uppercase = 0;
       break;
  case 0xB6:
       uppercase = 0;
       break;
  case 0x1D:
       ctrlKey = 1;
       break;
  case 0xE0:
       ctrlKey = 1;
       break;
  case 0x9D:
       ctrlKey = 0;
       break;     
  default:
     if(!ctrlKey) {
       *(CTRL_KEYPRESS_ADDRESS + 1 ) = '\0';
       *(CTRL_KEYPRESS_ADDRESS + 2)= 0x07;
     }
     if (key <= 88 && map[key] != '\0') {
       if (ctrlKey) {
         *(CTRL_KEYPRESS_ADDRESS + 1) = '^';
         *(CTRL_KEYPRESS_ADDRESS + 2) = 0x07;
       }
       if (uppercase && map[key] >= 97 && map[key] <=122) {
         *(KEYPRESS_ADDRESS + 1)= (char)(map[key] -32);
         *(KEYPRESS_ADDRESS + 2) = 0x07;
       }
       else {
         *(KEYPRESS_ADDRESS +1) = (char)(map[key]);
         *(KEYPRESS_ADDRESS +2) = 0x07;
       }
     }
  };
  outportb(0x20, 0x20);
}

void init_ps2_controller() {
  set_ir_in_idt(33, keyboard_interrupt_service_routine);
}
