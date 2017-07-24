
/*  Define info about PCI */

#ifndef PCI_H
#define PCI_H 1

#include <stdint.h>

/*  Bit field representing PCI config address */
typedef struct _pci_config_addr_t
{
        unsigned int reserved  : 2;
        unsigned int reg_num   : 6;
        unsigned int func_num  : 3;
        unsigned int dev_num   : 5;
        unsigned int bus_num   : 8;
        unsigned int reserved2 : 7;
        unsigned int enable    : 1;
} pci_config_addr_t;



#endif // PCI_H
