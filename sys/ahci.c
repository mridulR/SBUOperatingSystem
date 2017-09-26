#include <sys/ahci.h>
#include <sys/pci.h>
#include <sys/kprintf.h>

#define  SATA_SIG_ATA  0x00000101  // SATA drive
#define  SATA_SIG_ATAPI  0xEB140101  // SATAPI drive
#define  SATA_SIG_SEMB  0xC33C0101  // Enclosure management bridge
#define  SATA_SIG_PM      0x96690101  // Port multiplier
#define ATA_CMD_READ_DMA_EX 0x25

#define AHCI_DEV_NULL        0
#define HBA_PORT_DET_PRESENT 3
#define HBA_PORT_IPM_ACTIVE  1

#define ATA_DEV_BUSY 0x80
#define ATA_DEV_DRQ 0x08
 

// Check device type
static int check_type(hba_port_t *port)
{
  uint32_t ssts = port->ssts;
  
  uint8_t ipm = (ssts >> 8) & 0x0F;
  uint8_t det = ssts & 0x0F;
  
  //kprintf("\n ssts = %p ipm= %p det=%p sig=%p cmd = %p\n", port->ssts, ipm, det, port->sig, port->cmd);
 
  if (det != HBA_PORT_DET_PRESENT)  // Check drive status
    return AHCI_DEV_NULL;
  if (ipm != HBA_PORT_IPM_ACTIVE)
    return AHCI_DEV_NULL;
  
  switch (port->sig)
  {
  case SATA_SIG_ATAPI:
    return AHCI_DEV_SATAPI;
  case SATA_SIG_SEMB:
    return AHCI_DEV_SEMB;
  case SATA_SIG_PM:
    return AHCI_DEV_PM;
  default:
    return AHCI_DEV_SATA;
  }
}


void probe_port(hba_mem_t *abar)
{
  // Search disk in impelemented ports
  uint32_t pi = abar->pi;
        //kprintf("\nabar->cap = %p ghc = %p ir = %p abar->pi = %p\n", abar->cap, abar->ghc, abar->is_rwc, abar->pi);
  int i = 0;
  while (i<MAX_PORT_CNT)
  {
    if (pi & 1)
    {
      int dt = check_type((hba_port_t *)&abar->ports[i]);
      if (dt == AHCI_DEV_SATA)
      {
        kprintf("SATA drive found at port %d\n", i);
      }
      else if (dt == AHCI_DEV_SATAPI)
      {
        kprintf("SATAPI drive found at port %d\n", i);
      }
      else if (dt == AHCI_DEV_SEMB)
      {
        kprintf("SEMB drive found at port %d\n", i);
      }
      else if (dt == AHCI_DEV_PM)
      {
        kprintf("PM drive found at port %d\n", i);
      }
    }
    pi >>= 1;
    i++;
  }
}
 
void init_ahci() {

  Pci_dev_info *devInfo = NULL;
  for(int i = 0; i< getPCIDevInfoCount(); ++i) {

    devInfo = getPCIDevInfo(i);

    if(devInfo->vendorId == 32902 && devInfo->deviceId == 10530 &&
                devInfo->classCode == 1  && devInfo->subClass == 6) {

      kprintf("\nAHCI VendorID: %d DeviceId: %d Class: %d SubClass: %d \nBar[4]: %p Bar[5]: %p\n",\
               devInfo->vendorId, devInfo->deviceId, devInfo->classCode,\
               devInfo->subClass, devInfo->bar4, devInfo->bar5);
      break;
    }
  }
  if(devInfo!= NULL) {
    unsigned long bar5 = (unsigned long)(devInfo->bar5);
    hba_mem_t *abar = (hba_mem_t *)bar5;
    probe_port(abar);
  }
}
