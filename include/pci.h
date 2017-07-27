
#ifndef PCI_H
#define PCI_H

#include <stdint.h>

// bitfield containing info about a device

typedef union _pci_dev_t {

        uint32_t bits;
        struct {
                unsigned int always_zero : 2;
                unsigned int reg_num     : 6;
                unsigned int func_num    : 3;
                unsigned int dev_num     : 5;
                unsigned int bus_num     : 8;
                unsigned int reserved    : 7;
                unsigned int enable      : 1;
        };

} pci_dev_t;

/* Ports */

#define PCI_CONFIG_ADDR      0xCF8
#define PCI_CONFIG_DATA      0xCFC

/*  Offsets */
#define PCI_VEND_ID          0x00
#define PCI_DEV_ID           0x02
#define PCI_CMD              0x04
#define PCI_STATUS           0x06
#define PCI_REV_ID           0x08
#define PCI_PROG_IF          0x09
#define PCI_SUBCLASS         0x0a
#define PCI_CLASS            0x0b
#define PCI_CACHE_LINE_SIZE  0x0c
#define PCI_LATENCY_TIMER    0x0d
#define PCI_HEADER_TYPE      0x0e
#define PCI_BIST             0x0f
#define PCI_BAR0             0x10
#define PCI_BAR1             0x14
#define PCI_BAR2             0x18
#define PCI_BAR3             0x1C
#define PCI_BAR4             0x20
#define PCI_BAR5             0x24
#define PCI_INT_LINE         0x3C
#define PCI_SECOND_BUS       0x09

/* Device Types */
#define PCI_HEADER_TYPE_DEV       0
#define PCI_HEADER_TYPE_BRIDGE    1
#define PCI_HEADER_TYPE_CARDBUS   2
#define PCI_TYPE_BRIDGE           0x0604
#define PCI_TYPE_SATA             0x0106
#define PCI_NONE                  0xFFFF

/*  Other macros */
#define PCI_DEVICE_PER_BUS 32
#define PCI_FUNC_PER_DEV   32

/*  Function Declaration */

uint32_t    pci_read(pci_dev_t, uint32_t);
void        pci_write(pci_dev_t, uint32_t, uint32_t);
uint32_t    pci_get_dev_type(pci_dev_t);
uint32_t    pci_get_secnd_bus(pci_dev_t);
uint32_t    pci_reach_end(pci_dev_t);
pci_dev_t   pci_scan_funct(uint16_t, uint16_t, uint32_t,
                           uint32_t, uint32_t, int);
pci_dev_t   pci_scan_dev(uint16_t, uint16_t, uint32_t, uint32_t, int);
pci_dev_t   pci_scan_bus(uint16_t, uint16_t, uint32_t, int);
pci_dev_t   pci_get_dev(uint16_t, uint16_t, int);

void        pci_init();

#endif // PCI_H
