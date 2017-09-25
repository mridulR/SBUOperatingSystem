#include <sys/ahci.h>
#include <sys/pci.h>
#include <sys/kprintf.h>

#define	SATA_SIG_ATA	0x00000101	// SATA drive
#define	SATA_SIG_ATAPI	0xEB140101	// SATAPI drive
#define	SATA_SIG_SEMB	0xC33C0101	// Enclosure management bridge
#define	SATA_SIG_PM	    0x96690101	// Port multiplier
#define ATA_CMD_READ_DMA_EX 0x25

#define AHCI_DEV_NULL        0
#define HBA_PORT_DET_PRESENT 3
#define HBA_PORT_IPM_ACTIVE  1
/*

ssts = 0         ipm= 0 det= 0 sig=0          No drive found at port 3
ssts = 0         ipm= 0 det= 0 sig= 72        No drive found at port 4
ssts = 0         ipm= 0 det= 0 sig=0          No drive found at port 6
ssts =  5D5BC97F ipm= 9 det= F sig= B4        No drive found at port 13
ssts =  53C35B05 ipm= B det= 5 sig= FF18948   No drive found at port 19
ssts =  FFFFFEB6 ipm= E det= 6 sig= E83F8B08  No drive found at port 21
ssts =  8788B48  ipm= B det= 8 sig= C4048B49  No drive found at port 31
*/
#define ATA_DEV_BUSY 0x80
#define ATA_DEV_DRQ 0x08
 
/*int read(hba_port_t *port, uint32_t startl, uint32_t starth, uint32_t count, WORD *buf)
{
	port->is = (uint32_t)-1;		// Clear pending interrupt bits
	int spin = 0; // Spin lock timeout counter
	int slot = find_cmdslot(port);
	if (slot == -1)
		return 0;
 
	hba_cmd_header_t *cmdheader = (hba_cmd_header_t*)port->clb;
	cmdheader += slot;
	cmdheader->cfl = sizeof(FIS_REG_H2D)/sizeof(uint32_t);	// Command FIS size
	cmdheader->w = 0;		// Read from device
	cmdheader->prdtl = (WORD)((count-1)>>4) + 1;	// PRDT entries count
 
	hba_cmd_tbl_t *cmdtbl = (hba_cmd_tbl_t*)(cmdheader->ctba);
	memset(cmdtbl, 0, sizeof(hba_cmd_tbl_t) +
 		(cmdheader->prdtl-1)*sizeof(hba_prdt_entry_t));
 
	// 8K bytes (16 sectors) per PRDT
	for (int i=0; i<cmdheader->prdtl-1; i++)
	{
		cmdtbl->prdt_entry[i].dba = (uint32_t)buf;
		cmdtbl->prdt_entry[i].dbc = 8*1024;	// 8K bytes
		cmdtbl->prdt_entry[i].i = 1;
		buf += 4*1024;	// 4K words
		count -= 16;	// 16 sectors
	}
	// Last entry
	cmdtbl->prdt_entry[i].dba = (uint32_t)buf;
	cmdtbl->prdt_entry[i].dbc = count<<9;	// 512 bytes per sector
	cmdtbl->prdt_entry[i].i = 1;
 
	// Setup command
	FIS_REG_H2D *cmdfis = (FIS_REG_H2D*)(&cmdtbl->cfis);
 
	cmdfis->fis_type = FIS_TYPE_REG_H2D;
	cmdfis->c = 1;	// Command
	cmdfis->command = ATA_CMD_READ_DMA_EX;
 
	cmdfis->lba0 = (uint8_t)startl;
	cmdfis->lba1 = (uint8_t)(startl>>8);
	cmdfis->lba2 = (uint8_t)(startl>>16);
	cmdfis->device = 1<<6;	// LBA mode
 
	cmdfis->lba3 = (uint8_t)(startl>>24);
	cmdfis->lba4 = (uint8_t)starth;
	cmdfis->lba5 = (uint8_t)(starth>>8);
 
	cmdfis->countl = LOBYTE(count);
	cmdfis->counth = HIBYTE(count);
 
	// The below loop waits until the port is no longer busy before issuing a new command
	while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000)
	{
		spin++;
	}
	if (spin == 1000000)
	{
		kprintf("Port is hung\n");
		return 0;
	}
 
	port->ci = 1<<slot;	// Issue command
 
	// Wait for completion
	while (1)
	{
		// In some longer duration reads, it may be helpful to spin on the DPS bit 
		// in the PxIS port field as well (1 << 5)
		if ((port->ci & (1<<slot)) == 0) 
			break;
		if (port->is & HBA_PxIS_TFES)	// Task file error
		{
			kprintf("Read disk error\n");
			return 0;
		}
	}
 
	// Check again
	if (port->is & HBA_PxIS_TFES)
	{
		kprintf("Read disk error\n");
		return 0;
	}
 
	return 1;
}
 
// Find a free command list slot
int find_cmdslot(hba_port_t *port)
{
	// If not set in SACT and CI, the slot is free
	uint32_t slots = (m_port->sact | m_port->ci);
	for (int i=0; i<cmdslots; i++)
	{
		if ((slots&1) == 0)
			return i;
		slots >>= 1;
	}
	kprintf("Cannot find free command list entry\n");
	return -1;
}*/

// Check device type
static int check_type(hba_port_t *port)
{
  /*uint32_t ssts = port->ssts;
  
  uint8_t ipm = (ssts >> 8) & 0x0F;
  uint8_t det = ssts & 0x0F;
  
  //kprintf("\n ssts = %p ipm= %p det=%p sig=%p cmd = %p\n", port->ssts, ipm, det, port->sig, port->cmd);
 
  if (det != HBA_PORT_DET_PRESENT)	// Check drive status
  	return AHCI_DEV_NULL;
  if (ipm != HBA_PORT_IPM_ACTIVE)
  	return AHCI_DEV_NULL;*/
  
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
            //hba_port_t *val = (0xFFFFFFFFFFFFFFFF & ((uint64_t *)abar->ports[i]));
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
			else
			{
				kprintf("No drive found at port %d\n", i);
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
    //uint64_t bar5 = ((devInfo->bar5) & ~0x1FFF);
    uint32_t val = (devInfo->bar5);
    hba_mem_t *abar =(hba_mem_t *) (&val);
    //kprintf(" mem Bar: %p, mem.first = %p", abar, abar->cap);
    probe_port(abar);
  }
}
