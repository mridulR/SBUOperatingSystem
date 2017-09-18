#ifndef _PIT_H
#define _PIT_H

#include <sys/types.h>

/*  
   Structure of the command instruction :
   ------------------------------------------
   | 7  | 6  | 5  | 4  | 3  |  2 | 1  |  0  |
   ------------------------------------------
   
   BIT 6,7    - CHANNEL 0         (00) 
                CHANNEL 1         (01)
                CHANNEL 2         (10)
                READ BACK COMMAND (11)
  
   BIT 4,5    - Latch count value command  (00)  
                Access mode: lobyte only   (01)  
                Access mode: hibyte only   (10)  
                Access mode: lobyte/hibyte (11)  

   BIT 3,2,1  - TERMINAL COUNT   (000), 
                ONESHOT          (001), 
                RATE GENERATOR   (010, 110),
                SQUARE WAVE GEN  (011, 111), 
                SOFTWARE_TRIGGER (100), 
                HARDWARE_TRIGGER (101) 

   BIT 0      - 0 for binary, 1 for BCD
*/

//! BITS 1-3        
#define PIT_TERMINALCOUNT     0x0    
#define PIT_ONESHOT           0x2    
#define PIT_RATE_GEN          0x4    
#define PIT_SQUARE_WAVE_GEN   0x6    
#define PIT_SOFTWARE_TRIGGER  0x8    
#define PIT_HARDWARE_TRIGGER  0xA    

//! BIT 0
#define PIT_BINARY      0x0
#define PIT_BCD         0x1    

//! BIT 6-7       
#define PIT_CHANNEL_0 0x40
#define PIT_CHANNEL_1 0x41
#define PIT_CHANNEL_2 0x42
#define PIT_COMMAND   0x43

//! Initializes the pit
void init_pit();

//! Gets the global pit counter 
uint32_t get_pit_count();

#endif
