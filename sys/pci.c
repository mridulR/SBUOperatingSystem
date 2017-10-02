#include <sys/pci.h>
#include <sys/ahci.h>
#include <sys/memset.h>

#define MAX_PCI_DEVICES 256

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA    0xCFC 

#define REGISTER_OFFSET_VENDOR          0x00
#define REGISTER_OFFSET_STATUS          0x04
#define REGISTER_OFFSET_CLASS           0x08
#define REGISTER_OFFSET_BIST            0x0C
#define REGISTER_OFFSET_BAR_ADDR_0      0x10
#define REGISTER_OFFSET_BAR_ADDR_1      0x14
#define REGISTER_OFFSET_BAR_ADDR_2      0x18
#define REGISTER_OFFSET_BAR_ADDR_3      0x1C
#define REGISTER_OFFSET_BAR_ADDR_4      0x20
#define REGISTER_OFFSET_BAR_ADDR_5      0x24
#define REGISTER_OFFSET_CS_POINTER      0x28
#define REGISTER_OFFSET_SUBSYSTEM       0x2C
#define REGISTER_OFFSET_ROM_BASE_ADDR   0x30
#define REGISTER_OFFSET_RESERVED_0      0x34
#define REGISTER_OFFSET_RESERVED_1      0x38
#define REGISTER_OFFSET_INTERRUPT_FLAGS 0x3C

#define HBA_MEM_ADDRESS_2  0x2F000000
#define HBA_MEM_ADDRESS    0x000A6000
// 0x000A0000 => 640

/*

Good Read: https://en.wikibooks.org/wiki/X86_Assembly/X86_Architecture

Register naming conventions: 

 ---------------------------------------------------------------------------------------------------------------
|  register  | Accumulator | Counter | Data  | Base  | Stack Pointer | Stack Base Pointer | Source | Destination|
 ---------------------------------------------------------------------------------------------------------------
|  64-bit    | RAX         | RCX     | RDX   | RBX   | RSP           | RBP                | RSI    | RDI        |
|  32-bit    | EAX         | ECX     | EDX   | EBX   | ESP           | EBP                | ESI    | EDI        |
|  16-bit    | AX          | CX      | DX    | BX    | SP            | BP                 | SI     | DI         |
|  8-bit     | AH AL       | CH CL   | DH DL | BH BL |               |                    |        |            |
 ---------------------------------------------------------------------------------------------------------------

*/


//! Currently we handle only 8 devices
Pci_dev_info devInfo[MAX_PCI_DEVICES] = {0};

static int devCount = 0;

Pci_dev_info* getPCIDevInfo(uint8_t index) {
  return &devInfo[index];
}

uint32_t getPCIDevInfoCount() {
  return devCount; 
}

uint32_t inportl(uint16_t port)
{
  uint32_t ret;
  __asm__ volatile
  (
    "inl %%dx, %%eax\n"
    :"=a"(ret)
    :"d"(port)
  );
  return ret;
}

void outportl(uint16_t port, uint32_t value)                                                                                      
{
  __asm__ volatile
  (
    "outl %%eax, %%dx\n"
    : 
    :"d" (port), "a" (value)
  );
}

uint32_t pciConfigReadWord(uint8_t bus, uint8_t slot,
                            uint8_t func, uint8_t offset){
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint32_t tmp = 0;
 
    /* create configuration address as per Figure 1 */
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));
 
    /* write out the address */
    outportl(PCI_CONFIG_ADDRESS, address);

    /* read in the data */
    /* (offset & 2) * 8) = 0 will choose the first word of the 32 bits register
    // The below math is not required here. Let's return the 32bit word and shift
    // to extract respective values.
    tmp = (uint16_t)((inportl(PCI_CONFIG_DATA) >> ((offset & 2) * 8)) & 0xffff);*/
    
    tmp = (uint32_t)(inportl(PCI_CONFIG_DATA)  & 0xffffffff);
    return (tmp);
 }

uint16_t pciCheckVendor(uint8_t bus, uint8_t slot, uint8_t funct) {

  uint32_t word     = pciConfigReadWord(bus, slot, funct, REGISTER_OFFSET_VENDOR);
  uint16_t vendorId   = word & 0xFFFF;
  uint16_t deviceId = (word >> 16) & 0xFFFF;

  /* try and read the first configuration register. Since there are no */
  /* vendors that == 0xFFFF, it must be a non-existent device. */
  if (vendorId != 0xFFFF) {
     devInfo[devCount].vendorId = vendorId;
     devInfo[devCount].deviceId = deviceId;
     devInfo[devCount].bus = (uint32_t)bus;
     devInfo[devCount].func= (uint32_t)funct;
     devInfo[devCount].slot= (uint32_t)slot;

     word = pciConfigReadWord(bus, slot, funct, REGISTER_OFFSET_STATUS);
     devInfo[devCount].command = (word & 0xFFFF);
     devInfo[devCount].status  = ((word >> 16) & 0xFFFF);

     word = pciConfigReadWord(bus, slot, funct, REGISTER_OFFSET_CLASS);
     devInfo[devCount].revisionId = (word & 0xFF);
     devInfo[devCount].progIf     = ((word >> 8)  & 0xFF);
     devInfo[devCount].subClass   = ((word >> 16) & 0xFF);
     devInfo[devCount].classCode  = ((word >> 24) & 0xFF);

     kprintf("\nFound device:  V: %d D: %d C: %d S: %d  F: %d!!!", vendorId, deviceId, devInfo[devCount].classCode, devInfo[devCount].subClass, funct);
     word = pciConfigReadWord(bus,slot, funct, REGISTER_OFFSET_BIST);
     devInfo[devCount].cacheLineSize = (word & 0xFF);
     devInfo[devCount].latencyTimer  = ((word >> 8)  & 0xFF);
     devInfo[devCount].headerType    = ((word >> 16) & 0xFF);
     devInfo[devCount].bist          = ((word >> 24) & 0xFF);

     word = pciConfigReadWord(bus,slot, funct, REGISTER_OFFSET_BAR_ADDR_0);
     devInfo[devCount].bar0 = word & 0xFFFFFFFF;

     word = pciConfigReadWord(bus,slot, funct, REGISTER_OFFSET_BAR_ADDR_1);
     devInfo[devCount].bar1 = word & 0xFFFFFFFF;

     word = pciConfigReadWord(bus,slot, funct, REGISTER_OFFSET_BAR_ADDR_2);
     devInfo[devCount].bar2 = word & 0xFFFFFFFF;

     word = pciConfigReadWord(bus,slot, funct, REGISTER_OFFSET_BAR_ADDR_3);
     devInfo[devCount].bar3 = word & 0xFFFFFFFF;

     word = pciConfigReadWord(bus,slot, funct, REGISTER_OFFSET_BAR_ADDR_4);
     devInfo[devCount].bar4 = word & 0xFFFFFFFF;
     
     //TODO: Please don't use hard coded address later on.
     // Fix this when we implement paging
     uint32_t address;
     //uint32_t func = 0;

     unsigned long addr_temp = (unsigned long)HBA_MEM_ADDRESS;
     memset((uint32_t*) addr_temp, '\0', sizeof(hba_mem_t));

     address = (uint32_t)((bus << 16) | (slot << 11) |
              (funct << 8) | (REGISTER_OFFSET_BAR_ADDR_5 & 0xfc) | ((uint32_t)0x80000000));
 
     outportl(PCI_CONFIG_ADDRESS, address);
     outportl(PCI_CONFIG_DATA, addr_temp);

     word = pciConfigReadWord(bus,slot, funct, REGISTER_OFFSET_BAR_ADDR_5);
     devInfo[devCount].bar5 = word & 0xFFFFFFFF;

     word = pciConfigReadWord(bus,slot, funct, REGISTER_OFFSET_CS_POINTER);
     devInfo[devCount].cardBusCISPtr = word & 0xFFFFFFFF;

     word = pciConfigReadWord(bus,slot, funct, REGISTER_OFFSET_SUBSYSTEM);
     devInfo[devCount].subsystemVendorId = (word  & 0xFFFF);
     devInfo[devCount].subsystemId       = ((word >> 16) & 0xFFFF);
 
     word = pciConfigReadWord(bus,slot, funct, REGISTER_OFFSET_ROM_BASE_ADDR);
     devInfo[devCount].expansionBaseROMAddr = word & 0xFFFFFFFF;

     word = pciConfigReadWord(bus,slot, funct, REGISTER_OFFSET_RESERVED_0);
     devInfo[devCount].capabilitiesPtr = (word & 0xFFFF);
     devInfo[devCount].reserved0       = ((word >> 16) & 0xFFFF);
 
     word = pciConfigReadWord(bus,slot, funct, REGISTER_OFFSET_RESERVED_1);
     devInfo[devCount].reserved1= word & 0xFFFFFFFF;

     word = pciConfigReadWord(bus,slot, funct, REGISTER_OFFSET_INTERRUPT_FLAGS);
     devInfo[devCount].interruptLine = (word & 0xFF);
     devInfo[devCount].interruptPin  = ((word >> 8)  & 0xFF);
     devInfo[devCount].minGrant      = ((word >> 16) & 0xFF);
     devInfo[devCount].maxLatency    = ((word >> 24) & 0xFF);

     /*kprintf("\nB: %d S: %d V: %d D: %d S: %d C: %d P: %d R: %d C: %d S: %d L: %d T: %d B: %d H: %d ", bus, slot, 
             devInfo[devCount].vendorId, devInfo[devCount].deviceId, 
             devInfo[devCount].status, devInfo[devCount].command,
             devInfo[devCount].progIf, devInfo[devCount].revisionId,
             devInfo[devCount].classCode, devInfo[devCount].subClass,
             devInfo[devCount].cacheLineSize, devInfo[devCount].latencyTimer, 
             devInfo[devCount].bist, devInfo[devCount].headerType);
      
      
      kprintf("\nB0: %p B1: %p B2: %p B3: %p B4: %p B5: %p IL: %d IP: %d LT: %d G: %d word: %d",
             devInfo[devCount].bar0, devInfo[devCount].bar1, 
             devInfo[devCount].bar2, devInfo[devCount].bar3, 
             devInfo[devCount].bar4, devInfo[devCount].bar5, 
             devInfo[devCount].interruptLine, devInfo[devCount].interruptPin, 
             devInfo[devCount].maxLatency, devInfo[devCount].minGrant, word); 

       kprintf("\nV: %d D: %d P: %d R: %d C: %d S: %d  word= %d\n",
             devInfo[devCount].vendorId, devInfo[devCount].deviceId, 
             devInfo[devCount].progIf, devInfo[devCount].revisionId,
             devInfo[devCount].classCode, devInfo[devCount].subClass, word);*/
             /*devInfo[devCount].cacheLineSize, devInfo[devCount].latencyTimer, 
             devInfo[devCount].bist, devInfo[devCount].headerType , word);
     kprintf("devCount = %d ", devCount);*/
     devCount++;
  }
  return (vendorId);
}
/*
00	Device ID	Vendor ID
04	Status	Command
08	Class code	Subclass	Prog IF	Revision ID
0C	BIST	Header type	Latency Timer	Cache Line Size
10	Base address #0 (BAR0)
14	Base address #1 (BAR1)
18	Base address #2 (BAR2)
1C	Base address #3 (BAR3)
20	Base address #4 (BAR4)
24	Base address #5 (BAR5)
28	Cardbus CIS Pointer
2C	Subsystem ID	Subsystem Vendor ID
30	Expansion ROM base address
34	Reserved	Capabilities Pointer
38	Reserved
3C	Max latency	Min Grant	Interrupt PIN	Interrupt Line
*/
void init_pci_devInfo() {

  for(int bus = 0; bus < 256; ++bus) {
    for(int deviceId = 0; deviceId < 32; ++deviceId) {
      for (int funct = 0; funct < 8; ++funct)
       pciCheckVendor(bus, deviceId, funct);  
    }
  }
}
