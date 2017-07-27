
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

#endif // PCI_H
