
#include <stdint.h>
#include <printf.h>
#include <low_io.h>
#include <pci.h>
#include <malloc.h>
#include <timer.h>

inline ide_buf_t
new_buf()
{
        ide_buf_t buf = {0};
        return buf;
}

inline atapi_packet_t
new_atapi_packet()
{
        atapi_packet_t pack = {0xA8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        return pack;
}

ide_dev_t*
new_ide_device(unsigned int bar0, unsigned int bar1, unsigned int bar2,
               unsigned int bar3, unsigned int bar4, unsigned char type)
{
        ide_dev_t* dev = (ide_dev_t*) malloc(sizeof(ide_dev_t));

        if (type == ATA_PRIMARY){

                dev->base_addr = (bar0 & 0xFFFFFFFC) + ATA1      * (!bar0);
                dev->ctrl_addr = (bar1 & 0xFFFFFFFC) + ATA1_CTRL * (!bar1);
                dev->bmide     = (bar4 & 0xFFFFFFFC) + 0;

        } else {

                dev->base_addr = (bar2 & 0xFFFFFFFC) + ATA2      * (!bar2);
                dev->ctrl_addr = (bar3 & 0xFFFFFFFC) + ATA2_CTRL * (!bar3);
                dev->bmide     = (bar4 & 0xFFFFFFFC) + 8;

        }

        return dev;
}

/*
  Something like this:
  ide_dev_t* master = new_ide_device(ATA1, ATA);
  ide_dev_t* slave  = new_ide_device(ATA2, ATAPI);
 */

unsigned char
ide_read(ide_dev_t* dev, uint8_t reg)
{
        unsigned char result;
        if (reg > 0x07 && reg < 0x0C)
                ide_write(dev, STATUS_CMD, 0x80 | dev->ctrl_addr + NEIN);
        if (reg < 0x08)
                result = inb(dev->base_addr + reg - 0x00);
        else if (reg < 0x0C)
                result = inb(dev->base_addr + reg - 0x06);
        else if (reg < 0x0E)
                result = inb(dev->ctrl_addr  + reg - 0x0A);
        else if (reg < 0x16)
                result = inb(dev->bmide + reg - 0x0E);
        if (reg > 0x07 && reg < 0x0C)
                ide_write(dev, STATUS_CMD, dev->ctrl_addr + NEIN);
        return result;
}

void
ide_write(ide_dev_t* dev, uint8_t reg, uint8_t data)
{
        if (reg > 0x07 && reg < 0x0C)
                ide_write(dev, STATUS_CMD, 0x80 | dev->ctrl_addr + NEIN);
        if (reg < 0x08)
                outb(dev->base_addr + reg - 0x00, data);
        else if (reg < 0x0C)
                outb(dev->base_addr + reg - 0x06, data);
        else if (reg < 0x0E)
                outb(dev->ctrl_addr + reg - 0x0A, data);
        else if (reg < 0x16)
                outb(dev->bmide + reg - 0x0E, data);
        if (reg > 0x07 && reg < 0x0C)
                ide_write(dev, STATUS_CMD, dev->ctrl_addr + NEIN);
}

void
ide_read_buf(ide_dev_t* dev, uint8_t reg, unsigned int buffer, unsigned int count)
{
        if (reg > 0x07 && reg < 0x0C)
                ide_write(dev, STATUS_CMD, 0x80 | dev->ctrl_addr + NEIN);
        asm volatile ("pushw %es; pushw %ax; movw %ds, %ax; movw %ax, %es; popw %ax");

        if (reg < 0x08)
                insl(dev->base_addr + reg - 0x00, buffer, count);
        else if (reg < 0x0C)
                insl(dev->base_addr + reg - 0x06, buffer, count);
        else if (reg < 0x0E)
                insl(dev->ctrl_addr  + reg - 0x0A, buffer, count);
        else if (reg < 0x16)
                insl(dev->bmide + reg - 0x0E, buffer, count)

        asm volatile ("popw %es;");
        if (reg > 0x07 && reg < 0x0C)
                ide_write(dev, STATUS_CMD, dev->ctrl_addr + NEIN);
}

unsigned char
ide_poll(ide_dev_t* dev, unsigned int check)
{
        // delay
        for (int i = 0; i < 4; i++)
                ide_read(dev, 2); // alternate status reg

        while (ide_read(dev, STATUS_CMD) & BSY); // wait until status is no longer busy

        if (check){

                unsigned char state = ide_read(dev, STATUS_CMD);
                if (state & ERR)
                        return 1; // error

                if (state & DF)
                        return 2; // disk fault

                if ((state & DRQ) == 0)
                        return 3; // DRQ should be set

        }

        return 0;
}

ide_dev_t* channels[2]; // two channels
ide_dev_t* devices[4];  // accept 4 devices

static void
__pci_init(unsigned int bar0, unsigned int bar1, unsigned int bar2, unsigned int bar3, unsigned int bar4)
{
        // populate devices
        channels[ATA_PRIMARY  ] = new_ide_device(bar0, bar1, bar2, bar3, bar4, ATA_PRIMARY);
        channels[ATA_SECONDARY] = new_ide_device(bar0, bar1, bar2, bar3, bar4, ATA_SECONDARY);

        // disable IRQ
        ide_write(channels[ATA_PRIMARY]  , 0x0C, 2); // write to control register
        ide_write(channels[ATA_SECONDARY], 0x0C, 2);

        int i, j, count = 0;
        for (i = 0; i < 2; i++){
                for (j = 0; j < 2;){

                        unsigned char err  = 0;
                        unsigned char type = ATA;
                        unsigned char status;

                        // select drive
                        ide_dev_t* tmp_dev = NULL;
                        tmp_dev = devices[0];

                        // assume that it will be a dud
                        tmp_dev->reserved = 0;

                        // select drive
                        ide_write(channels[i], 0x06, 0xA0 | (j << 4));
                        timer_wait(1);

                        // identify drive
                        ide_write(channels[i], STATUS_CMD, ATA_ID_DEV);
                        timer_wait(1);

                        // wait until ready
                        if(ide_read(channels[i], STATUS_CMD) == 0) continue; // if status == 0, no devices

                        while(1){
                                status = ide_read(channels[i], STATUS_CMD);

                        }

                }
        }

}

void
pci_init()
{
        __pci_init();
}
