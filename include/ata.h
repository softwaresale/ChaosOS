
/* This file is the header definitons for the ATA driver */

#ifndef ATA_H
#define ATA_H 1

#include <stdint.h>
#include <low_io.h>

/* Port values */
#define ATA1           (0x1F0)     // base ATA1 value
#define ATA1_DATA      (ATA1)      // data port for ATA1
#define ATA1_FEAT_ERR  (ATA1 + 1)  // ATA features / error port (for ATAPI)
#define ATA1_SECT_CT   (ATA1 + 2)  // ATA1 number of sectors to r/w
#define ATA1_LBA_LO    (ATA1 + 3)  // LBA lo value
#define ATA1_LBA_MID   (ATA1 + 4)  // LBA mid value
#define ATA1_LBA_HI    (ATA1 + 5)  // LBA hi value
#define ATA1_DRIVE_PRT (ATA1 + 6)  // Select a drive or head
#define ATA1_CMD_STAT  (ATA1 + 7)  // Write commands or get the current status

#define ATA2           (0x3F6)
#define ATA2_DATA      (ATA2)      // data port for ATA1
#define ATA2_FEAT_ERR  (ATA2 + 1)  // ATA features / error port (for ATAPI)
#define ATA2_SECT_CT   (ATA2 + 2)  // ATA1 number of sectors to r/w
#define ATA2_LBA_LO    (ATA2 + 3)  // LBA lo value
#define ATA2_LBA_MID   (ATA2 + 4)  // LBA mid value
#define ATA2_LBA_HI    (ATA2 + 5)  // LBA hi value
#define ATA2_DRIVE_PRT (ATA2 + 6)  // Select a drive or head
#define ATA2_CMD_STAT  (ATA2 + 7)  // Write commands or get the current status

/* Status bytes */
#define ERR 0 // Error occured
#define DRQ 3 // set when PIO is ready for data transfer
#define SRV 4 // overlapped mode request
#define DF  5 // Drive fault (doesn't set err)
#define RDY 6 // Bit is clear when drive is spun down or after error (set otherwise)
#define BSY 7 // Indicates if the drive is busy

#define NEIN 1 // set this to stop devise from sending interrupts
#define SRST 2 // Do a software reset
#define HOB  7 // Read last high order byte from LBA

#define MASTER           0xE0
#define SLAVE            0xF0
#define ATA_WRITE        0x30
#define ATA_READ         0x20
#define ATA_CACHE_FLUSH  0xE7
#define ATA_ID_DEV       0xEC

#define SECTOR_SIZE      512

// macro function for 400ns delay
#define DELAY(x) (for (int i = 0; i < 5; i++){ inb(ATA1_CMD_STAT); })

/* Types of drives */
typedef enum _device_types_t
{
        ATA,
        ATAPI,
        SATA,
        STATPI
} dev_types_t;

typedef enum _ata_types_t
{
        _ATA1,
        _ATA2
} ata_types_t;

typedef struct _ata_drive_t
{
        dev_types_t   type;       // type of drive (using ATA as default for now)
        unsigned int  base_addr;  // base address of drive (ATA1 OR ATA2)
        unsigned int  status;     // status byte
        unsigned char select_bit; // 0xe0 or 0xf0
        char*         buffer;     // buffer for reading data into

        // port offsets
        unsigned int data_port;
        unsigned int err_feats;
        unsigned int sect_ct;
        unsigned int lba_lo;
        unsigned int lba_mid;
        unsigned int lba_hi;
        unsigned int drive_head;
        unsigned int cmd_status;

} ata_drive_t;

/* Function Decls begin */

ata_drive_t*
ata_new(ata_types_t);

/* Reads data from an ata drive
 * ata_drive_t* -- drive to read from
 * int -- lba
 * int -- number of sectors to read */
int
ata_read(ata_drive_t*, uint32_t, unsigned int);

/* Writes data to ATA

   ata_drive_t -- drive to write to
   int -- offset
   int -- size
   char* -- buffer to write
 */
int
ata_write(ata_drive_t*, uint32_t, unsigned int, char*);

#endif // ATA_H
