#ifndef _PCI_H
#define _PCI_H

#include <sys/types.h>
#include <sys/kprintf.h>

struct pci_dev_info{
  uint32_t bus;
  uint32_t slot;
  uint32_t func;
  uint16_t deviceId;
  uint16_t vendorId;
  uint16_t status;
  uint16_t command;
  uint8_t classCode;
  uint8_t subClass;
  uint8_t progIf;
  uint8_t revisionId;
  uint8_t bist;
  uint8_t headerType;
  uint8_t latencyTimer;
  uint8_t cacheLineSize;
  uint32_t bar0;
  uint32_t bar1;
  uint32_t bar2;
  uint32_t bar3;
  uint32_t bar4;
  uint32_t bar5;
  uint32_t cardBusCISPtr;
  uint16_t subsystemId;
  uint16_t subsystemVendorId;
  uint32_t expansionBaseROMAddr;
  uint16_t reserved0;
  uint16_t capabilitiesPtr;
  uint32_t reserved1;
  uint8_t  maxLatency;
  uint8_t minGrant;
  uint8_t interruptPin;
  uint8_t interruptLine;
} __attribute__((packed));

typedef struct pci_dev_info Pci_dev_info;

void init_pci_devInfo();

uint32_t pciConfigReadWord (uint8_t bus, uint8_t slot,
                            uint8_t func, uint8_t offset);

uint16_t pciCheckVendor(uint8_t bus, uint8_t slot, uint8_t funct);

uint32_t inportl(uint16_t port);

void outportl(uint16_t port, uint32_t value);

Pci_dev_info* getPCIDevInfo(uint8_t index);

uint32_t getPCIDevInfoCount();

#endif
