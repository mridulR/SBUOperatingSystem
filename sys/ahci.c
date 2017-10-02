#include <sys/ahci.h>
#include <sys/pci.h>
#include <sys/kprintf.h>
#include <sys/memset.h>


#define  SATA_SIG_ATA  0x00000101  // SATA drive
#define  SATA_SIG_ATAPI  0xEB140101  // SATAPI drive
#define  SATA_SIG_SEMB  0xC33C0101  // Enclosure management bridge
#define  SATA_SIG_PM      0x96690101  // Port multiplier

#define HBA_PORT_CMD_ST 0x1
#define HBA_PORT_CMD_FRE 0x10
#define HBA_PORT_CMD_FR 0x4000
#define RESET_CMD_ST 0xfffffffe
#define RESET_CMD_FRE 0xffffffef
#define HBA_PORT_CMD_CR 0x8000

#define ATA_CMD_READ_DMA_EX 0x25
#define ATA_CMD_WRITE_DMA_EX 0x35

#define AHCI_BASE   0x400000    // 4M
#define AHCI_DEV_NULL        0

#define HBA_PORT_DET_PRESENT 3
#define HBA_PORT_IPM_ACTIVE  1
#define COMMAND_SLOTS 32

#define ATA_DEV_BUSY 0x80
#define ATA_DEV_DRQ 0x08

#define TRUE  1 
#define FALSE 0 

#define BLOCK_BASE_ADDRESS 0x00000100
#define NUM_BLOCKS 1 
#define BLOCK_SIZE 4096 

#define REGISTER_OFFSET_BAR_ADDR_5      0x24

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA    0xCFC 

typedef uint8_t  BOOL;
typedef uint8_t  BYTE;
typedef uint32_t DWORD;
typedef uint16_t WORD;

// TODO: This value will be updated with second SATA drive.
// Update code accordingly to read/write to SATA drives
uint32_t g_SATA_PORT_INDEX = 0xFFFFFFFF; 

// Start command engine
void start_cmd(hba_port_t *port)
{
    // Wait until CR (bit15) is cleared
    while (port->cmd & HBA_PxCMD_CR);
 
    // Set FRE (bit4) and ST (bit0)
    port->cmd |= HBA_PxCMD_FRE;
    port->cmd |= HBA_PxCMD_ST; 
}
 
// Stop command engine
void stop_cmd(hba_port_t *port)
{
    // Clear ST (bit0)
    port->cmd &= ~HBA_PxCMD_ST;
 
    // Wait until FR (bit14), CR (bit15) are cleared
    while(1)
    {
        if (port->cmd & HBA_PxCMD_FR)
            continue;
        if (port->cmd & HBA_PxCMD_CR)
            continue;
        break;
    }
 
    // Clear FRE (bit4)
    port->cmd &= ~HBA_PxCMD_FRE;
}


// Find a free command list slot
int find_cmdslot(hba_port_t *port)
{
    // If not set in SACT and CI, the slot is free
    DWORD slots = (port->sact | port->ci);
    for (int i=0; i< COMMAND_SLOTS; i++)
    {
        if ((slots&1) == 0)
            return i;
        slots >>= 1;
    }
    kprintf("Cannot find free command list entry\n");
    return -1;
}

BOOL write(hba_port_t *port, DWORD startl, DWORD starth, DWORD count, WORD *buf)
{
    port->is_rwc = (DWORD)-1;        // Clear pending interrupt bits
    int spin = 0; // Spin lock timeout counter
    int slot = find_cmdslot(port);
    kprintf("Write Checkpoint 0\n");
    if (slot == -1)
        return FALSE;
 
    hba_cmd_header_t *cmdheader = (hba_cmd_header_t*)port->clb;
    cmdheader += slot;
    cmdheader->cfl = sizeof(fis_reg_h2d_t)/sizeof(DWORD);    // Command FIS size
    cmdheader->w = 1;        // Write from device
    cmdheader->prdtl = (WORD)((count-1)>>4) + 1;    // PRDT entries count
 
    kprintf("Write Checkpoint 1\n");
    hba_cmd_tbl_t *cmdtbl = (hba_cmd_tbl_t*)(cmdheader->ctba);
    memset(cmdtbl, 0, sizeof(hba_cmd_tbl_t) +
         (cmdheader->prdtl-1)*sizeof(hba_prdt_entry_t));

    // 8K bytes (16 sectors) per PRDT
    int i=0;
    for (; i<cmdheader->prdtl-1; i++)
    {
        cmdtbl->prdt_entry[i].dba = (DWORD)((uint64_t)buf);
        cmdtbl->prdt_entry[i].dbc = 8*1024;    // 8K bytes
        cmdtbl->prdt_entry[i].i = 1;
        buf += 4*1024;    // 4K words
        count -= 16;    // 16 sectors
    }
    // Last entry
    cmdtbl->prdt_entry[i].dba = (DWORD)((uint64_t)buf);
    cmdtbl->prdt_entry[i].dbc = count<<9;    // 512 bytes per sector
    cmdtbl->prdt_entry[i].i = 1;
 
    // Setup command
    fis_reg_h2d_t *cmdfis = (fis_reg_h2d_t*)(&cmdtbl->cfis);
 
    cmdfis->fis_type = FIS_TYPE_REG_H2D;
    cmdfis->c = 1;    // Command
    cmdfis->command = ATA_CMD_WRITE_DMA_EX;
 
    cmdfis->lba0 = (BYTE)startl;
    cmdfis->lba1 = (BYTE)(startl>>8);
    cmdfis->lba2 = (BYTE)(startl>>16);
    cmdfis->device = 1<<6;    // LBA mode
 
    cmdfis->lba3 = (BYTE)(startl>>24);
    cmdfis->lba4 = (BYTE)starth;
    cmdfis->lba5 = (BYTE)(starth>>8);
 
    cmdfis->count = count;
 
    kprintf("Write Checkpoint 2\n");
    // The below loop waits until the port is no longer busy before issuing a new command
    while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000)
    {
        spin++;
    }
    if (spin == 1000000)
    {
        kprintf("Port is hung\n");
        return FALSE;
    }
    kprintf("Write Checkpoint 3\n");
 
    port->ci = 1<<slot;    // Issue command
 
    // Wait for completion
    while (1)
    {
        // In some longer duration reads, it may be helpful to spin on the DPS bit 
        // in the PxIS port field as well (1 << 5)
        if ((port->ci & (1<<slot)) == 0) 
            break;
        if (port->is_rwc & HBA_PxIS_TFES)    // Task file error
        {
            kprintf("Read disk error\n");
            return FALSE;
        }
    }
 
    // Check again
    if (port->is_rwc & HBA_PxIS_TFES)
    {
        kprintf("Read disk error\n");
        return FALSE;
    }
    kprintf("Write ret true\n");
 
    return TRUE;
}

BOOL read(hba_port_t *port, DWORD startl, DWORD starth, DWORD count, WORD *buf)
{
    port->is_rwc = (DWORD)-1;        // Clear pending interrupt bits
    int spin = 0; // Spin lock timeout counter
    int slot = find_cmdslot(port);
    if (slot == -1)
        return FALSE;
 
    hba_cmd_header_t *cmdheader = (hba_cmd_header_t*)port->clb;
    cmdheader += slot;
    cmdheader->cfl = sizeof(fis_reg_h2d_t)/sizeof(DWORD);    // Command FIS size
    cmdheader->w = 0;        // Read from device
    cmdheader->prdtl = (WORD)((count-1)>>4) + 1;    // PRDT entries count
 
    hba_cmd_tbl_t *cmdtbl = (hba_cmd_tbl_t*)(cmdheader->ctba);
    memset(cmdtbl, 0, sizeof(hba_cmd_tbl_t) +
         (cmdheader->prdtl-1)*sizeof(hba_prdt_entry_t));

    // 8K bytes (16 sectors) per PRDT
    int i=0;
    for (; i<cmdheader->prdtl-1; i++)
    {
        cmdtbl->prdt_entry[i].dba = (DWORD)((uint64_t)buf);
        cmdtbl->prdt_entry[i].dbc = 8*1024;    // 8K bytes
        cmdtbl->prdt_entry[i].i = 1;
        buf += 4*1024;    // 4K words
        count -= 16;    // 16 sectors
    }
    // Last entry
    cmdtbl->prdt_entry[i].dba = (DWORD)((uint64_t)buf);
    cmdtbl->prdt_entry[i].dbc = count<<9;    // 512 bytes per sector
    cmdtbl->prdt_entry[i].i = 1;
 
    // Setup command
    fis_reg_h2d_t *cmdfis = (fis_reg_h2d_t*)(&cmdtbl->cfis);
 
    cmdfis->fis_type = FIS_TYPE_REG_H2D;
    cmdfis->c = 1;    // Command
    cmdfis->command = ATA_CMD_READ_DMA_EX;
 
    cmdfis->lba0 = (BYTE)startl;
    cmdfis->lba1 = (BYTE)(startl>>8);
    cmdfis->lba2 = (BYTE)(startl>>16);
    cmdfis->device = 1<<6;    // LBA mode
 
    cmdfis->lba3 = (BYTE)(startl>>24);
    cmdfis->lba4 = (BYTE)starth;
    cmdfis->lba5 = (BYTE)(starth>>8);
 
    cmdfis->count = count;
 
    // The below loop waits until the port is no longer busy before issuing a new command
    while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000)
    {
        spin++;
    }
    if (spin == 1000000)
    {
        kprintf("Port is hung\n");
        return FALSE;
    }
 
    port->ci = 1<<slot;    // Issue command
 
    // Wait for completion
    while (1)
    {
        // In some longer duration reads, it may be helpful to spin on the DPS bit 
        // in the PxIS port field as well (1 << 5)
        if ((port->ci & (1<<slot)) == 0) 
            break;
        if (port->is_rwc & HBA_PxIS_TFES)    // Task file error
        {
            kprintf("Read disk error\n");
            return FALSE;
        }
    }
 
    // Check again
    if (port->is_rwc & HBA_PxIS_TFES)
    {
        kprintf("Read disk error\n");
        return FALSE;
    }
 
    return TRUE;
}

void port_rebase(hba_port_t *port, int portno)
{
    //stop_cmd(port);    // Stop command engine
 
    // Command list offset: 1K*portno
    // Command list entry size = 32
    // Command list entry maxim count = 32
    // Command list maxim size = 32*32 = 1K per port
    port->clb = AHCI_BASE + (portno<<10);
    memset((void*)(port->clb), 0, 1024);
 
    // FIS offset: 32K+256*portno
    // FIS entry size = 256 bytes per port
    port->fb = AHCI_BASE + (32<<10) + (portno<<8);
    memset((void*)(port->fb), 0, 256);
 
    // Command table offset: 40K + 8K*portno
    // Command table size = 256*32 = 8K per port
    hba_cmd_header_t *cmdheader = (hba_cmd_header_t*)(port->clb);
    for (int i=0; i<32; i++)
    {
        cmdheader[i].prdtl = 8;    // 8 prdt entries per command table
                    // 256 bytes per command table, 64+16+48+16*8
        // Command table offset: 40K + 8K*portno + cmdheader_index*256
        cmdheader[i].ctba = AHCI_BASE + (40<<10) + (portno<<13) + (i<<8);
        memset((void*)cmdheader[i].ctba, 0, 256);
    }
 
    //start_cmd(port);    // Start command engine
}
 
// Check device type
static int check_type(hba_port_t *port)
{

  /*uint32_t ssts = port->ssts;
  
  uint8_t ipm = (ssts >> 8) & 0x0F;
  uint8_t det = ssts & 0x0F;
  
  //kprintf("\n ssts = %p ipm= %p det=%p sig=%p cmd = %p\n", port->ssts, ipm, det, port->sig, port->cmd);
 
  if (det != HBA_PORT_DET_PRESENT)  // Check drive status
    return AHCI_DEV_NULL;
  if (ipm != HBA_PORT_IPM_ACTIVE)
    return AHCI_DEV_NULL; */
  
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

      WORD cmdStatus = (abar->ports[i].cmd) & HBA_PORT_CMD_ST;
      WORD cmdFre = (abar->ports[i].cmd)&HBA_PORT_CMD_FRE;

      if (dt == AHCI_DEV_SATA)
      {
        //TODO: Fix this check
        //if(g_SATA_PORT_INDEX == 0xFFFFFFFF || i < 2){
          g_SATA_PORT_INDEX = i;
          kprintf("SATA drive found at port %d and g_SATA_PORT_INDEX = %d \n", i, g_SATA_PORT_INDEX);
        //}
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
      if(cmdFre!=0||cmdStatus!=0){
        if(cmdStatus!=0 && i< 2){
          abar->ports[i].cmd&=RESET_CMD_ST;
        }
        if(cmdFre!=0 && i < 2){
	      (abar->ports[i].cmd)&=RESET_CMD_FRE;
        }
        i--;
        continue;
      }
    }
    pi >>= 1;
    i++;
  }

  //for(int i = 0; i<MAX_PORT_CNT; ++i) {
  g_SATA_PORT_INDEX = 0;
  port_rebase(&(abar->ports[g_SATA_PORT_INDEX]), g_SATA_PORT_INDEX);
  //}

  for(int i = 0;i<=1;i++){
    while(abar->ports[i].cmd & HBA_PORT_CMD_CR);
    abar->ports[i].cmd |= HBA_PORT_CMD_FRE;
    abar->ports[i].cmd |= HBA_PORT_CMD_ST;
    kprintf("HERE : %d", i);
  }

}

int str_len(char *ch) {
  int len = 1;
  char *trav = ch;
  while (*trav != '\0') {
    trav++;
    len++;
   }
  return len; 
}

 
void init_ahci() {
  
  hba_mem_t *abar = NULL;
  Pci_dev_info *devInfo = NULL;
  for(int i = 0; i< getPCIDevInfoCount(); ++i) {

    devInfo = getPCIDevInfo(i);

    if(devInfo->classCode == 1  && devInfo->subClass == 6) {
 
       kprintf("\nAHCI VendorID: %d DeviceId: %d Class: %d SubClass: %d \nBar[4]: %p Bar[5]: %p\n",\
                devInfo->vendorId, devInfo->deviceId, devInfo->classCode,\
               devInfo->subClass, devInfo->bar4, devInfo->bar5);
       break;
     }
  }

  if(!devInfo)
      kprintf("\n AHCI NOT FOUND !!!");

  if(devInfo!= NULL) {

    unsigned long bar5 = (unsigned long)(devInfo->bar5);
    abar = (hba_mem_t *)bar5;
    probe_port(abar);
    if(g_SATA_PORT_INDEX != -1) {
      char *writeBuffer[1];
      
      uint64_t addr = (BLOCK_BASE_ADDRESS);
      char *ptr = (char *) (uint64_t)addr;
      
      uint32_t wstartl = 0;
      uint32_t wstarth = 0;
      uint32_t rstartl = 0;
      uint32_t rstarth = 0;
      
      writeBuffer[0] = ptr;
      for(int i=0; i<NUM_BLOCKS; ++i) {
        memset(writeBuffer[0], '\0', BLOCK_SIZE);
        memset(writeBuffer[0], i+1, BLOCK_SIZE-1);
        g_SATA_PORT_INDEX = 0;
        write(&abar->ports[g_SATA_PORT_INDEX], wstartl, wstarth, 8, (WORD *)writeBuffer[0]);
        wstartl = wstartl + 8;
      }
    
      char *readBuffer[1];
      for(int i = 0; i< NUM_BLOCKS; ++i) {
        readBuffer[i] = NULL;
        int readCount = 0;
        //char byteVal = '\0';
        read(&abar->ports[g_SATA_PORT_INDEX], rstartl, rstarth, 8, (WORD *)readBuffer[0]);
        //byteVal = *readBuffer[0];
        rstartl = rstartl + 8;
        readCount = readCount + str_len(readBuffer[0]);
        //kprintf("BlockCount= %d ByteValue = %c BytesRead = %d \n", i+1, byteVal, readCount);
        //kprintf("Read Value = %s\n", readBuffer[0]);
      }
    } 
  }

}
