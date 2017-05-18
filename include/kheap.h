
#ifndef KHEAP_H
#define KHEAP_H 1

#include <stdint.h>
#include <stddef.h>
#include <paging.h>

#define OFFSETOF(TYPE, MEMBER)  __builtin_offsetof (TYPE, MEMBER)

typedef struct DList DList;

struct DList {

	DList *next;
	DList *prev;
};

typedef struct _chunk {

	DList all;
	int used;
	union {
		char data[0];
		DList free;
	}

} chunk_t;

enum
{
	NUM_SIZES = 32,
	ALIGN     = 4,
	MIN_SIZE  = sizeof(DList),
	HEADER_SIZE = OFFSETOF(chunk_t, data);
}

chunk_t* free_chunk[NUM_SIZES] = { NULL; }
size_t mem_free = 0;
size_t mem_used = 0;
size_t mem_meta = 0;
chunk_t* first  = NULL;
chunk_t* last   = NULL

void
mem_init(void* mem, size_t size);

void* 
kmalloc(size_t size);

void
kfree(void* mem);

#endif // KHEAP_H

