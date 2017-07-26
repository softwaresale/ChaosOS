
#include <stdint.h>
#include <stdio.h>
#include <low_io.h>
#include <pci.h>
#include <malloc.h>
#include <timer.h>

static uint64_t
swap_endian(uint64_t val)
{
        uint64_t x = val;

        x = (x & 0x00000000FFFFFFFF) << 32 | (x & 0xFFFFFFFF00000000) >> 32;
        x = (x & 0x0000FFFF0000FFFF) << 16 | (x & 0xFFFF0000FFFF0000) >> 16;
        x = (x & 0x00FF00FF00FF00FF) << 8  | (x & 0xFF00FF00FF00FF00) >> 8;

        return x;
}


ide_dev_t*
new_ide_channel(unsigned int bar0, unsigned int bar1, unsigned int bar2,
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

        dev->channel = NULL; // point to null

        return dev;
}

ide_dev_t*
new_ide_device(ide_dev_t* channel, unsigned char type, unsigned char master, unsigned int* buffer)
{
        ide_dev_t* tmp = (ide_dev_t*) malloc(sizeof(ide_dev_t));

        // set data
        tmp->reserved  = 1;
        tmp->channel   = channel;
        tmp->type      = type;
        tmp->drive     = master;
        tmp->sign      = *((unsigned short*)(buffer + ATA_DEVTYPE));
        tmp->cmdset    = *((unsigned int*)  (buffer + ATA_COMMANDSET));
        tmp->feats     = *((unsigned short*)(buffer + ATA_CAPAB));

        // set size
        if(tmp->cmdset & (1 << 26))
                tmp->size = *((unsigned int*)(buffer + ATA_MAX_LBA_EXT)); // 48-bit
        else
                tmp->size = *((unsigned int*)(buffer + ATA_MAX_LBA));     // 28/CHS

        // populate model
        for (int i = 0; i < 40; i += 2){
                tmp->model[i] = buffer[ATA_MODEL + i + 1];
                tmp->model[i + i] = buffer[ATA_MODEL + i];
        }

        tmp->model[40] = 0; // null terminate

        return tmp;
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
                insl(dev->bmide + reg - 0x0E, buffer, count);

        asm volatile ("popw %es;");
        if (reg > 0x07 && reg < 0x0C)
                ide_write(dev, STATUS_CMD, dev->ctrl_addr + NEIN);
}


unsigned char
ide_print_error(ide_dev_t* drive, unsigned char err)
{
        if (err == 0)
                return err;

        printf("IDE:");
        if (err == 1){ printf("- Device Fault\n        "); err = 19; }
        else if (err == 2){
                printf("Error encountered....\n");
                // TODO: Implement rest
        }

        return err;
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
        channels[ATA_PRIMARY  ] = new_ide_channel(bar0, bar1, bar2, bar3, bar4, ATA_PRIMARY);
        channels[ATA_SECONDARY] = new_ide_channel(bar0, bar1, bar2, bar3, bar4, ATA_SECONDARY);

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
                                if ((status & ERR)) {err = 1; break; } // error encountered
                                if (!(status & BSY) && (status % DRQ)) break; // all good
                        }

                        // probe for ATAPI
                        if (err != 0){
                                unsigned char cl = ide_read(channels[i], LBA_MID);
                                unsigned char ch = ide_read(channels[i], LBA_HI);

                                if (cl == 0x14 && ch == 0xEB)
                                        type = ATAPI;
                                else if (cl == 0x69 && ch == 0x96)
                                        type = ATAPI;
                                else
                                        continue; // unknown type (might not be a device)

                                ide_write(channels[i], STATUS_CMD, ATA_ID_PACKET);
                                timer_wait(1);
                        }

                        // read id space
                        unsigned int buffer[2048];
                        ide_read_buf(channels[i], DATA, buffer, 128);

                        // read device parameters
                        tmp_dev = new_ide_device(channels[i], type, j, buffer);

                        count++;
                } // for (j = 0...)
        } // for (i = 0...)

        for (i = 0; i < 4; i++){
                // if drive exists
                if (devices[i]->reserved == 1){
                        printf("Found Drive Type: %s (%dGB) -- %s\n",
                               (const char*[]){"ATA", "ATAPI"}[devices[i]->type],
                               devices[i]->size / 1024 / 1024 / 2,
                               devices[i]->model);
                }
        }

} // __pci_init

// dir -- direction (READ / WRITE)
// drive -- which device to write to
// lba -- lba address
// numsects -- number of sectors to read/write
// selector -- segment selector to read from
// edi -- segment offset

static int
ide_irq_invoked = 0;

unsigned char
ide_ata_access(ide_dev_t* dev, unsigned char dir, unsigned int lba,
               unsigned char numsects, unsigned short selector, unsigned int edi)
{
        unsigned char lba_mode;  // CHS 28/48
        unsigned char dma;       // DMA mode
        unsigned char cmd;       // command to write?
        unsigned char lba_io[6];
        ide_dev_t*    channel = dev->channel;
        unsigned int  drive   = dev->drive;
        unsigned int  words   = 256; // 512 bytes
        unsigned short cyl, i;
        unsigned char head, sect, err;

        // disabel IRQ
        ide_write(channel, 0x0C, channel->nein = (0x0) + 0x02);

        // Read some params
        if (lba >= 0x10000000){
                // LBA 48
                lba_mode = 2;

                lba_io[0] = (lba & 0x000000FF) >> 0;
                lba_io[1] = (lba & 0x0000FF00) >> 8;
                lba_io[2] = (lba & 0x00FF0000) >> 16;
                lba_io[3] = (lba & 0xFF000000) >> 24;
                lba_io[4] = 0; // not
                lba_io[5] = 0; // needed
                head = 0; // lower 4 bits of hddevsel are not used

        } else if (dev->feats & 0x200) {
                // LBA 28
                lba_mode = 1;

                lba_io[0] = (lba & 0x00000FF) >> 0;
                lba_io[1] = (lba & 0x000FF00) >> 8;
                lba_io[2] = (lba & 0x0FF0000) >> 16;
                lba_io[3] = 0;
                lba_io[4] = 0;
                lba_io[5] = 0;
                head      = (lba & 0xF000000) >> 24;

        } else {
                // CHS
                lba_mode = 0;

                sect      = (lba % 63) + 1;
                cyl       = (lba + 1 - sect) / (16 * 63);
                lba_io[0] = sect;
                lba_io[1] = (cyl >> 0) & 0xFF;
                lba_io[2] = (cyl >> 8) & 0xFF;
                lba_io[3] = 0;
                lba_io[4] = 0;
                lba_io[5] = 0;
                head      = (lba + 1 - sect) % (16 * 63) / 63; // head number is written to lower 4 bits
        }

        // poll status port
        while(ide_read(dev, STATUS_CMD) & BSY); // wait if busy

        // select address mode (LBA/CHS)
        if (!lba_mode)
                ide_write(channel, DRIVE, 0xA0 | (dev->drive << 4) | head); // drive & CHS
        else
                ide_write(channel, DRIVE, 0xE0 | (dev->drive << 4) | head); // drive & LBA

        // write params
        if (lba_mode == 2){
                ide_write(channel, 0x08, 0);
                ide_write(channel, 0x09, lba_io[3]);
                ide_write(channel, 0x0A, lba_io[4]);
                ide_write(channel, 0x0B, lba_io[5]);
        }
        ide_write(channel, SECT_CT, numsects);
        ide_write(channel, LBA_LO,  lba_io[0]);
        ide_write(channel, LBA_MID, lba_io[1]);
        ide_write(channel, LBA_HI,  lba_io[2]);

        // write command
        if (lba_mode == 0 && dir == 0) cmd = ATA_PIO_READ;
        if (lba_mode == 0 && dir == 1) cmd = ATA_PIO_WRITE;
        if (lba_mode == 1 && dir == 0) cmd = ATA_PIO_READ;
        if (lba_mode == 1 && dir == 1) cmd = ATA_PIO_WRITE;
        if (lba_mode == 2 && dir == 0) cmd = ATA_PIO_READ_EXT;
        if (lba_mode == 2 && dir == 1) cmd = ATA_PIO_WRITE_EXT;
        ide_write(channel, STATUS_CMD, cmd);

        // actually read and write


        if (dir){
                // PIO write
                for (i = 0; i < numsects; i++){
                        ide_poll(channel, 0); // poll
                        asm("pushw %ds");
                        asm("mov %%ax, %%ds" :: "a"(selector));
                        asm("rep outsw"::"c"(words), "d"(channel->base_addr), "S"(edi));
                        asm("popw %ds");
                }

                ide_write(channel, STATUS_CMD, (char[]){ ATA_CACHE_FLUSH, ATA_CACHE_FLUSH, ATA_CACHE_FLUSH_EXT }[lba_mode]);
                ide_poll(channel, 0); // poll

        } else {
                // PIO READ
                for (i = 0; i < numsects; i++){
                        if (err = ide_poll(channel, 1))
                                return err;
                        asm("pushw %es");
                        asm("mov %%ax, %%es" : : "a"(selector));
                        asm("rep insw" : : "c"(words), "d"(channel->base_addr), "D"(edi));
                        asm("popw %es");
                        edi += (words*2);
                }
        }

        return 0;
}

void
ide_read_sectors(ide_dev_t* drive, unsigned char numsects, unsigned int lba,
                 unsigned short es, unsigned int edi)
{
        unsigned char package[12];

        // check if device is present
        if (drive->reserved == 0) package[0] = 0x1; // drive not found

        // check if inputs are valid
        else if (((lba + numsects) > drive->size) && (drive->type == ATA))
                package[0] = 0x2; // invalid seek position

        // read in PIO mode through polling
        else {
                unsigned char err;
                if (drive->type == ATA){
                        err = ide_ata_access(drive, ATA_READ, lba, numsects, es, edi);
                } else {
                        // TODO: implement atapi
                }
                package[0] = ide_print_error(drive, err);
        }
}


void
ide_write_sectors(ide_dev_t* drive, unsigned char numsects, unsigned int lba,
                 unsigned short es, unsigned int edi)
{
        unsigned char package[12];

        // check if device is present
        if(drive->reserved == 0) package[0] = 0x1; // drive not found

        // check if inputs are valid
        else if (((lba + numsects) > drive->size) && (drive->type == ATA))
                package[0] = 0x2; // invalid seek position

        // read in PIO mode through polling
        else {
                unsigned char err;
                if (drive->type == ATA){
                        err = ide_ata_access(drive, ATA_WRITE, lba, numsects, es, edi);
                } else {
                        // TODO: implement atapi
                }
                package[0] = ide_print_error(drive, err);
        }
}


void
pci_init()
{
        __pci_init(0x1F0, 0x3F6, 0x170, 0x376, 0x000);
}
