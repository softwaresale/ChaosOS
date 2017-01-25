#ifndef KHEAP_H
#define KHEAP_H 1

#include <stdint.h>
#include <stddef.h>

/*
// points to the start of the kernel heap's memory
void* KHEAP_START = (void*) 0x10000;

// points to the end of the kernel heap's memory
void* KHEAP_END   = (void*) 0x100000;

// points to the end of the heap memory currently allocated
void* cur_heap    = (void*) 0x10000;
*/

// Each Memory block is represented with the below struct
typedef struct heap_block {

	uint32_t size;           // size of block
	int free;                // if free '0'=free '1'=taken
	struct heap_block* next; // points to next block
	struct heap_block* prev; // points to last block
	char data[1];            // points to actual requested allocation

} heap_block_t;

// BLOCK SIZE contains malloc metadata
// char data[1] in struct heap_block_t points to user requested
// allocation.
// It will have a size of 4 because of padding. Hence we delete
// 4 from the sizeof
#define BLOCK_SIZE (sizeof(heap_block_t)-4)

// align memory to 4 btytes
#define align(x) ((((x-1)>>2)<<2)+4)

// extend heap
heap_block_t* extend_heap(heap_block_t* last, uint32_t size);

// allocate a block
heap_block_t* allocate_block(heap_block_t* last, uint32_t size);

// split a block
void split_block(heap_block_t* last, uint32_t size);

// malloc and free
void* kmalloc(uint32_t size);

// free a block
void kfree(void* fr);

// join adjacent free blocks
void fuse_block(heap_block_t* last);

#endif
