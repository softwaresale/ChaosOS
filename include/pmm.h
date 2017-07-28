
#ifndef PMM_H
#define PMM_H

#include <kernel.h>
#include <stdint.h>

#define BLOCK_SIZE 4096
#define BLOCKS_PER_BUCKET 8

#define SETBIT(i) (bitmap[i / BLOCKS_PER_BUCKET] = bitmap[i / BLOCKS_PER_BUCKET] | (1 << (i % BLOCKS_PER_BUCKET)))
#define CLRBIT(i) (bitmap[i / BLOCKS_PER_BUCKET] = bitmap[i / BLOCKS_PER_BUCKET] & (~(1 << (i % BLOCKS_PER_BUCKET))))
#define ISSET(i) ((bitmap[i / BLOCKS_PER_BUCKET] >> (i % BLOCKS_PER_BUCKET)) & 0x1)
#define GET_BUCKET(i) (*((uint32_t) &bitmap[i / 32]))

#define BLOCK_ALIGN(addr) (((addr) & 0xFFFFF000) + 0x1000)

extern uint32_t end;

void pmm_init(uint32_t mem_size);

uint32_t allocate_block();

void free_block(uint32_t);

#endif // PMM_H
