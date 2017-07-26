
/* This file is the header definitons for the ATA driver */

#ifndef ATA_H
#define ATA_H 1

#include <stdint.h>
#include <low_io.h>

/*  Base register offsets */
#define DATA        0
#define FEAT_ERR    1
#define SECT_CT     2
#define LBA_LO      3
#define LBA_MID     4
#define LBA_HI      5
#define DRIVE       6
#define STATUS_CMD  7

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

#define ATA1_CTRL           (0x3F6)
#define ATA1_CTRL_DATA      (ATA1_CTRL)      // data port for ATA1
#define ATA1_CTRL_FEAT_ERR  (ATA1_CTRL + 1)  // ATA features / error port (for ATAPI)
#define ATA1_CTRL_SECT_CT   (ATA1_CTRL + 2)  // ATA1 number of sectors to r/w
#define ATA1_CTRL_LBA_LO    (ATA1_CTRL + 3)  // LBA lo value
#define ATA1_CTRL_LBA_MID   (ATA1_CTRL + 4)  // LBA mid value
#define ATA1_CTRL_LBA_HI    (ATA1_CTRL + 5)  // LBA hi value
#define ATA1_CTRL_DRIVE_PRT (ATA1_CTRL + 6)  // Select a drive or head
#define ATA1_CTRL_CMD_STAT  (ATA1_CTRL + 7)  // Write commands or get the current status

#define ATA2           (0x170   )
#define ATA2_DATA      (ATA2    )
#define ATA2_FEAT_ERR  (ATA2 + 1)
#define ATA2_SECT_CT   (ATA2 + 2)
#define ATA2_LBA_LO    (ATA2 + 3)
#define ATA2_LBA_MID   (ATA2 + 4)
#define ATA2_LBA_HI    (ATA2 + 5)
#define ATA2_DRIVE_PRT (ATA2 + 6)
#define ATA2_CMD_STAT  (ATA2 + 7)

#define ATA2_CTRL           (0x376        )
#define ATA2_CTRL_DATA      (ATA2_CTRL    )
#define ATA2_CTRL_FEAT_ERR  (ATA2_CTRL + 1)
#define ATA2_CTRL_SECT_CT   (ATA2_CTRL + 2)
#define ATA2_CTRL_LBA_LO    (ATA2_CTRL + 3)
#define ATA2_CTRL_LBA_MID   (ATA2_CTRL + 4)
#define ATA2_CTRL_LBA_HI    (ATA2_CTRL + 5)
#define ATA2_CTRL_DRIVE_PRT (ATA2_CTRL + 6)
#define ATA2_CTRL_CMD_STAT  (ATA2_CTRL + 7)


/* ATA Status bytes */
#define ERR 0 // Error occured
#define DRQ 3 // set when PIO is ready for data transfer
#define SRV 4 // overlapped mode request
#define DF  5 // Drive fault (doesn't set err)
#define RDY 6 // Bit is clear when drive is spun down or after error (set otherwise)
#define BSY 7 // Indicates if the drive is busy

#define NEIN 1 // set this to stop devise from sending interrupts
#define SRST 2 // Do a software reset
#define HOB  7 // Read last high order byte from LBA

/*  Commands */
#define MASTER               0xE0
#define SLAVE                0xF0
#define ATA_PIO_WRITE        0x30
#define ATA_PIO_READ         0x20
#define ATA_CACHE_FLUSH      0xE7
#define ATA_ID_DEV           0xEC
#define ATA_PACKET           0xA0
#define ATA_ID_PACKET        0xA1
#define ATAPI_READ           0xA8
#define ATAPI_EJECT          0x18

/*  ID SPACE */
#define ATA_DEVTYPE       0
#define ATA_CYLN          2
#define ATA_HEADS         6
#define ATA_SECTS         12
#define ATA_SERIAL        20
#define ATA_MODEL         54
#define ATA_CAPAB         98
#define ATA_FIELDVALID    106
#define ATA_MAX_LBA       120
#define ATA_COMMANDSET    164
#define ATA_MAX_LBA_EXT   200

/*  Channels */
#define ATA_PRIMARY   0x00
#define ATA_SECONDARY 0x01

/*  Direction macros */
#define ATA_READ  0x00
#define ATA_WRITE 0x01

#define ATA    0x00
#define ATAPI  0x01

/*  Macro Data */
#define SECTOR_SIZE      512

/*  Class codes */
#define LEGACY                  0x00 // device made before naming sys
#define MASS_STOR               0x01 // Mass storage controller
#define NET_CTRL                0x02 // Network controller
#define DISP_CTRL               0x03 // Display controller
#define MM_CTRL                 0x04 // Multimedia controller
#define MEM_CTRL                0x05 // Memory Controller
#define BRIDGE_DEV              0x06 // Bridge device
#define SIMP_COMM_CTRL          0x07 // Simple communications controllers
#define BASE_SYS_PERIPH         0x08 // Base System peripherals
#define INPUT_DEV               0x09 // Input devices
#define DOCK_STATION            0x0A // Docking station
#define PROCS                   0x0B // Processors
#define SERIAL_BUS_CTRL         0x0C // Serial bus controllers
#define WIFI_CTRL               0x0D // Wireless controllers
#define INTELLI_IO_CTRL         0x0E // Intelligent I/O controller
#define SATELLITE_COMM_CTRL     0x0F // Satellite communication controller
#define CRYPT_CTRL              0x10 // Decryption/Encryption controller
#define DATA_ACQ_SIG_PROC_CTRL  0x11 // Data acquisition and signal processing controllers
// reserved space
#define UNKNOWN                 0xFF // Unknown

/* Macro Functions */
// macro function for 400ns delay
#define DELAY(x) (for (int i = 0; i < 5; i++){ inb(ATA1_CMD_STAT); })

static uint64_t
swap_endian(uint64_t val)
{
        uint64_t x = val;

        x = (x & 0x00000000FFFFFFFF) << 32 | (x & 0xFFFFFFFF00000000) >> 32;
        x = (x & 0x0000FFFF0000FFFF) << 16 | (x & 0xFFFF0000FFFF0000) >> 16;
        x = (x & 0x00FF00FF00FF00FF) << 8  | (x & 0xFF00FF00FF00FF00) >> 8;

        return x;
}

typedef struct _channel_t
{
        unsigned short base;  // Base address of device
        unsigned short ctrl;  // Control base address
        unsigned short bmide; // bus master id
        unsigned char  nein;  // no interrupt
} channel_t;


typedef struct _ide_device_t
{
        //                            0             1
        unsigned char  reserved;  // flag ( drive exists)
        unsigned char  channel;   // primary or secondary
        unsigned char  base_addr; // base address
        unsigned char  ctrl_addr; // controll address
        unsigned char  bmide;     // bus master id
        unsigned char  drive;     // master  or slave drive
        unsigned short type;      // ATA          ATAPI
        unsigned short sign;      // signature
        unsigned short feats;     // features
        unsigned int   cmdset;    // set of commands
        unsigned int   size;      // size in sectors
        unsigned int   model[41]; // buffer containing model type

} ide_dev_t;

/*
  Note that ide_dev_t contains more information needed, so some info must
  be set to NULL if not used and checked upon usage
 */

typedef union _ide_dev_id_t
{
        struct {

                struct {
                        unsigned int vend_id         : 16;
                        unsigned int dev_id          : 16;
                } block_one;

                struct {
                        unsigned int cmd             : 16;
                        unsigned int status          : 16;
                } block_two;

                struct {
                        unsigned char revision_id     : 8;
                        unsigned char prog_if         : 8;
                        unsigned char subclass        : 8;
                        unsigned char class           : 8;
                } block_three;

                struct {
                        unsigned char cache_line_size : 8;
                        unsigned char latency_time    : 8;
                        unsigned char header_type     : 8;
                        unsigned char bist            : 8;
                } block_three;

                unsigned int bar0                     : 32;
                unsigned int bar1                     : 32;
                unsigned int bar2                     : 32;
                unsigned int bar3                     : 32;
                unsigned int bar4                     : 32;
                unsigned int bar5                     : 32;
                unsigned int cardbus_ptr              : 32;

                struct {
                        unsigned int subsys_vend_id   : 16;
                        unsigned int subsys_id        : 16;
                } block_four;

                unsigned int  expansion_rom_addr      : 32;
                unsigned char capabilities            :  8;
                unsigned int  reserved                : 56;

                struct {
                        unsigned char interrupt_line  : 8;
                        unsigned char interrupt_pin   : 8;
                        unsigned char min_grant       : 8;
                        unsigned char max_latency     : 8;
                } block_five;

        } base_id;

} ide_dev_id_t;

// four available devices
extern ide_dev_t devices[4];

// Type for standard ide buffer and function for creation
typedef unsigned char ide_buf_t[2048];

ide_buf_t
new_buf();

// type and function for creating a new atapi packet
typedef unsigned char atapi_packet_t[12];

atapi_packet_t
new_atapi_packet();

/* Function Decls begin */


ide_dev_t*
new_ide_device(unsigned int bar0, unsigned int bar1, unsigned int bar2,
               unsigned int bar3, unsigned int bar4);

unsigned char
ide_read(ide_dev_t*, uint8_t);

/*  Writes data to channel's register */
void
ide_write(ide_dev_t*, uint8_t, uint8_t);

void
ide_read_buf(ide_dev_t*, uint8_t, unsigned int, unsigned int);

unsigned char
ide_poll(ide_dev_t*, unsigned int);

void
pci_init();

#endif // ATA_H
