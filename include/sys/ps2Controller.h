#ifndef _PS2_CONTROLLER_H
#define _PS2_CONTROLLER_H

#include <sys/types.h>

//! PS2 Controller defs
//! PS/2 Controller IO Ports
#define PS2_Data_Port         0x60
#define PS2_Status_Register   0x64
#define PS2_Command_Register  0x64

//! Adding the Scan codes Set 2 


//! Initializes the PS2 Controller 
void init_ps2_controller();

#endif
