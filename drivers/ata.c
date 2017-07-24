
#include <ata.h>
#include <low_io.h>
#include <malloc.h>
#include <string.h>

// create a new ata drive structure

ata_drive_t
ata_new(ata_types_t type)
{
        ata_drive_t tmp;
        tmp.type = ATA; // default for now

        // set slave or master
        if (type == _ATA1){
                tmp.base_addr = ATA1;
                tmp.select_bit = 0xe0;
        } else {
                tmp.base_addr = ATA2;
                tmp.select_bit = 0xf0;
        }

        // set data ports
        tmp.data_port  = tmp.base_addr;
        tmp.err_feats  = (tmp.base_addr + 1);
        tmp.sect_ct    = (tmp.base_addr + 2);
        tmp.lba_lo     = (tmp.base_addr + 3);
        tmp.lba_mid    = (tmp.base_addr + 4);
        tmp.lba_hi     = (tmp.base_addr + 5);
        tmp.drive_head = (tmp.base_addr + 6);
        tmp.cmd_status = (tmp.base_addr + 7);

        tmp.buffer = NULL; // set pointer to null, allocate during read call

        return tmp;
}

int
ata_read(ata_drive_t* drive, uint32_t offset, unsigned int size)
{
        // allocate the drives buffer
        drive->buffer = malloc(size);

        // calculate lba
        uint32_t start        = offset / SECTOR_SIZE;
        uint32_t start_offset = offset % SECTOR_SIZE;
        uint32_t end          = (offset + size - 1) / SECTOR_SIZE;
        uint32_t end_offset   = (offset + size - 1) % SECTOR_SIZE;

        // reading data
        uint32_t counter = start;
        uint32_t readsize;
        uint32_t off, total = 0;

        // start reading data
        // select apropriate drive
        port_byte_put(drive->drive_head, drive->select_bit | (drive->base_addr << 4) | (start >> 24)&0x0f);

        // write null byte to error area
        port_byte_put(drive->err_feats, 0x00);

        // send the sector count
}
