#include <bmheap.h>
#include <paging.h>
#include <stdint.h>
#include <stddef.h>

#define uint32 uint32_t
#define uint8 uint8_t
#define uintptr intptr_t

page_directory_t* kpage_dir;

void 
kheap_init(kheap_t *heap) {
	heap->fblock = 0;
}
 
int 
kheap_add_block(kheap_t *heap, uintptr_t addr, 
				uint32_t size, uint32_t bsize){
	block_t	 *b;
	uint32_t bcnt;
	uint32_t x;
	uint8_t  *bm;
 
	b = (block_t*)addr;
	b->size = size - sizeof(block_t);
	b->bsize = bsize;
 
	b->next = heap->fblock;
	heap->fblock = b;
 
	bcnt = b->size / b->bsize;
	bm = (uint8_t*)&b[1];
 
	/* clear bitmap */
	for (x = 0; x < bcnt; ++x) {
			bm[x] = 0;
	}
 
	/* reserve room for bitmap */
	bcnt = (bcnt / bsize) * bsize < bcnt ? bcnt / bsize + 1 : bcnt / bsize;
	for (x = 0; x < bcnt; ++x) {
			bm[x] = 5;
	}
 
	b->lfb = bcnt - 1;
 
	b->used = bcnt;
 
	return 1;
}
 
static uint8_t 
kheap_get_nid(uint8_t a, uint8_t b) {
	uint8_t	c;	
	for (c = a + 1; c == b || c == 0; ++c);
	return c;
}
 
void*
kheap_alloc(kheap_t *heap, uint32_t size, int align, void* phys) {
	block_t	 *b;
	uint8_t	 *bm;
	uint32_t  bcnt;
	uint32_t  x, y, z;
	uint32_t  bneed;
	uint8_t	  nid;
 
	/* iterate blocks */
	for (b = heap->fblock; b; b = b->next) {
		/* check if block has enough room */
		if (b->size - (b->used * b->bsize) >= size) {
 
			bcnt = b->size / b->bsize;		
			bneed = (size / b->bsize) * b->bsize < size ? size / b->bsize + 1 : size / b->bsize;
			bm = (uint8_t*)&b[1];
 
			for (x = (b->lfb + 1 >= bcnt ? 0 : b->lfb + 1); x < b->lfb; ++x) {
				/* just wrap around */
				if (x >= bcnt) {
					x = 0;
				}		
 
				if (bm[x] == 0) {	
					/* count free blocks */
					for (y = 0; bm[x + y] == 0 && y < bneed && (x + y) < bcnt; ++y);
 
					/* we have enough, now allocate them */
					if (y == bneed) {
						/* find ID that does not match left or right */
						nid = kheap_get_nid(bm[x - 1], bm[x + y]);
 
						/* allocate by setting id */
						for (z = 0; z < y; ++z) {
							bm[x + z] = nid;
						}
 
						/* optimization */
						b->lfb = (x + bneed) - 2;
 
						/* count used blocks NOT bytes */
						b->used += y;

						uint32_t addr = x * b->bsize + (uintptr) &b[1];

						// add alignment and/or physical abilities
						uint32_t align_addr = (addr & 0xFFFFF000) + 0x1000;

						if (phys != 0){
							uint32_t t = addr;
							if(align)
								t = align_addr;
							*phys = (void*) virtual2phys(kpage_dir, (void*) t);
						}

						if (align)
							return align_addr;

						return addr;

					}
 
					/* x will be incremented by one ONCE more in our FOR loop */
					x += (y - 1);
					continue;
				}
			}
		}
	}
 
	return 0;
}
 
void 
kheap_free(kheap_t *heap, void *ptr) {
	block_t	*b;	
	uintptr	ptroff;
	uint32	bi, x;
	uint8	*bm;
	uint8	id;
	uint32	max;
 
	for (b = heap->fblock; b; b = b->next) {
		if ((uintptr)ptr > (uintptr)b && (uintptr)ptr < ((uintptr)b + sizeof(block_t) + b->size)) {
			/* found block */
			ptroff = (uintptr)ptr - (uintptr)&b[1];  /* get offset to get block */
			/* block offset in BM */
			bi = ptroff / b->bsize;
			/* .. */
			bm = (uint8*)&b[1];
			/* clear allocation */
			id = bm[bi];
			/* oddly.. GCC did not optimize this */
			max = b->size / b->bsize;
			for (x = bi; bm[x] == id && x < max; ++x) {
				bm[x] = 0;
			}
			/* update free block count */
			b->used -= x - bi;
			return;
		}
	}
 
	/* this error needs to be raised or reported somehow */
	return;
}
