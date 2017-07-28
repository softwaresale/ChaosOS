
/* bit-map based heap */

#ifndef BMHEAP_H
#define BMHEAP_H 1

	
#include <stdint.h>
#include <stddef.h>
#include <paging.h>
	
// used for handling heap blocks
	
typedef struct _block {
	struct _block* next;
	uint32_t       size;
	uint32_t	   used;
	uint32_t	   bsize;
    uint32_t       lfb;
} block_t;
 
typedef struct _kheap {
	block_t* fblock;
} kheap_t;
	
void 
kheap_init(kheap_t *heap);

int 
kheap_add_block(kheap_t *heap, uintptr_t addr, 
				uint32_t size, uint32_t bsize);

static uint8_t 
kheap_get_nid(uint8_t a, uint8_t b);

void*
kheap_alloc(kheap_t *heap, uint32_t size, int, void*);

void 
kheap_free(kheap_t *heap, void *ptr);

#endif // BMHEAP_H
