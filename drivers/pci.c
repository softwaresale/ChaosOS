
#include <pci.h>
#include <stdio.h>
#include <stdint.h>
#include <low_io.h>

// static variables
uint32_t pci_size_map[100]; // array of pci size?
pci_dev_t dev_zero = {0};   // empty pci device
int fail = 0;

/*  Start defining functions */

/*  Read data from pci */
uint32_t
pci_read(pci_dev_t dev, uint32_t field)
{
        // most significant 6 bytes
        dev.reg_num = (field & 0xFC) >> 2;

        dev.enable = 1; // enable device
        outl(PCI_CONFIG_ADDR, dev.bits); // write data to configuration address

        uint32_t size = pci_size_map[field]; // how big should this field be
        if (size == 1){
                // get first bytes
                uint8_t v = inb(PCI_CONFIG_DATA + (field & 3)); // get 3rd byte (little endian)
                return v;
        } else if (size == 2){
                uint16_t v = inw(PCI_CONFIG_DATA + (field & 2));
                return v;
        } else if (size == 4){
                uint32_t v = inl(PCI_CONFIG_DATA);
                return v;
        }

        return 0xffff; // nothing found
}

/*  Write data to pci */
void
pci_write(pci_dev_t dev, uint32_t field, uint32_t data)
{
        dev.reg_num = (field & 0xFC) >> 2;
        dev.enable  = 1;

        // tell what we want to write
        outl(PCI_CONFIG_ADDR, dev.bits);

        // actually write
        outl(PCI_CONFIG_DATA, data);
}

/*  Get device type */
uint32_t
pci_get_dev_type(pci_dev_t dev)
{
        /*
          Type overview

              Device Class      Device Subclass
          |-----------------| |-----------------|
          0000 0000 0000 0000 0000 0000 0000 0000
         */
        uint32_t type = pci_read(dev, PCI_CLASS) << 8;
        return type | pci_read(dev, PCI_SUBCLASS);
}

uint32_t
pci_get_secnd_bus(pci_dev_t dev)
{
        return pci_read(dev, PCI_SECOND_BUS);
}

uint32_t
pci_reach_end(pci_dev_t dev)
{
        return pci_read(dev, PCI_HEADER_TYPE);
}

/*
 * These function recursivly scan over all devices connected to bus
 * If the vend id and dev id match, device found
 */

pci_dev_t
pci_scan_funct(uint16_t vend_id, uint16_t dev_id, uint32_t bus, uint32_t dev,
               uint32_t func, int dev_type)
{
        // create tempory device and set it's values to given parameters
        pci_dev_t tmp_dev = {0};
        tmp_dev.bus_num   = bus;
        tmp_dev.dev_num   = dev;
        tmp_dev.func_num  = func;

        // if PCI pridge, get bust its connected to and keep going
        if(pci_get_dev_type(tmp_dev) == PCI_TYPE_BRIDGE)
                pci_scan_bus(vend_id, dev_id, pci_get_secnd_bus(tmp_dev), dev_type);

        // if type matches, we found device (return it)
        if (dev_type == -1 || dev_type == pci_get_dev_type(tmp_dev)){
                uint32_t devid = pci_read(tmp_dev, PCI_DEV_ID);
                uint32_t vendid = pci_read(tmp_dev, PCI_VEND_ID);
                if (dev_id == devid && vend_id == vendid)
                        return tmp_dev; // right device
        }

        return dev_zero; // didn't find device
}

pci_dev_t
pci_scan_device(uint16_t vend_id, uint16_t dev_id, uint32_t bus, uint32_t device, int dev_type)
{
        pci_dev_t dev = {0};
        dev.bus_num   = bus;
        dev.dev_num   = device;

        if (pci_read(dev, PCI_VEND_ID) == PCI_NONE)
                return dev_zero; // device is nothing

        pci_dev_t t = pci_scan_funct(vend_id, dev_id, bus, device, 0, dev_type);
        if (t.bits)
                return t;

        if (pci_reach_end(dev))
                return dev_zero;

        for (int _func = 1; _func < PCI_FUNC_PER_DEV; _func++){
                if(pci_read(dev, PCI_VEND_ID) != PCI_NONE){
                        t = pci_scan_funct(vend_id, dev_id, bus, device, _func, dev_type);
                        if (t.bits)
                                return t;
                }
        }

        return dev_zero;
}

pci_dev_t
pci_scan_bus(uint16_t vend_id, uint16_t dev_id, uint32_t bus, int dev_type)
{
        for (int device = 0; device < PCI_DEVICE_PER_BUS; device++){
                pci_dev_t tmp = pci_scan_device(vend_id, dev_id, bus, device, dev_type);
                if(tmp.bits)
                        return tmp;
        }

        return dev_zero;
}

pci_dev_t
pci_get_dev(uint16_t vend_id, uint16_t dev_id, int dev_type)
{
        pci_dev_t tmp = pci_scan_bus(vend_id, dev_id, 0, dev_type);
        if (tmp.bits)
                return tmp; // bust found

        // handle multiple pci host controllers
        if(pci_reach_end(dev_zero)){
                printf("PCI get devide failed...\n");
                int fail = 1; // it failed
        }

        for (int _func = 1; _func < PCI_FUNC_PER_DEV; _func++){
                pci_dev_t dev = {0};
                dev.func_num = _func;

                if (pci_read(dev, PCI_VEND_ID) == PCI_NONE)
                        break; // dead device

                tmp = pci_scan_bus(vend_id, dev_id, _func, dev_type);
                if(tmp.bits)
                        return tmp;
        }

        return dev_zero;
}

void
pci_init()
{
        // fill size map
        pci_size_map[PCI_VEND_ID]         = 2;
        pci_size_map[PCI_DEV_ID]          = 2;
        pci_size_map[PCI_CMD]             = 2;
        pci_size_map[PCI_STATUS]          = 2;
        pci_size_map[PCI_SUBCLASS]        = 1;
        pci_size_map[PCI_CLASS]           = 1;
        pci_size_map[PCI_CACHE_LINE_SIZE] = 1;
        pci_size_map[PCI_LATENCY_TIMER]   = 1;
        pci_size_map[PCI_HEADER_TYPE]     = 1;
        pci_size_map[PCI_BIST]            = 1;
        pci_size_map[PCI_BAR0]            = 4;
        pci_size_map[PCI_BAR1]            = 4;
        pci_size_map[PCI_BAR2]            = 4;
        pci_size_map[PCI_BAR3]            = 4;
        pci_size_map[PCI_BAR4]            = 4;
        pci_size_map[PCI_BAR5]            = 4;
        pci_size_map[PCI_INT_LINE]        = 1;
        pci_size_map[PCI_SECOND_BUS]      = 1;

        if(fail)
                printf("PCI initilization failed...\n");
        else
                printf("PCI initilization complete\n");

}
