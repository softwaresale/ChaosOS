
#include <pmm.h>
#include <stdint.h>
#include <stdio.h>
#include <memory.h>

uint8_t* bitmap = (uint8_t*)(&end);
uint8_t* mem_start;
uint32_t total_blocks;
uint32_t bitmap_size;

static uint32_t
__first_free_block()
{
        uint32_t i;
        for (i = 0; i < total_blocks; i++){
                if(!ISSET(i))
                        return i;
        }
        return (uint32_t) -1;
}

void
pmm_init(uint32_t mem_size)
{
        total_blocks = mem_size / BLOCK_SIZE;

        bitmap_size = total_blocks / BLOCKS_PER_BUCKET;

        if(bitmap_size * BLOCKS_PER_BUCKET < total_blocks)
                bitmap_size++;

        memset(bitmap, 0, bitmap_size);

        mem_start = (uint8_t*) BLOCK_ALIGN(((uint32_t)(bitmap + bitmap_size)));
}

uint32_t
allocate_block()
{
        uint32_t free_block = __first_free_block();
        SETBIT(free_block);
        return free_block;
}

void
free_block(uint32_t block)
{
        CLRBIT(block);
}
