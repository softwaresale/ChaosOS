/* This handles memory management */

#ifndef KHEAP_H
#define KHEAP_H 1

#include <stdint.h>
#include <stddef.h>
#include <paging.h>

/* Info about a block */
struct _block
{
	uint32_t       address;    //!< Address of block in memory
	int            used;       //!< 0 if unsed, 1 if used
	size_t         block_size; //!< Size of the block
	struct _block* next;       //!< Block in front of
	struct _block* prev;       //!< Block behind
};

typedef struct _block block_t;

/* Info about the heap encapulated */
struct _kheap
{
	uint32_t placement_address; //!< Starting address for new address
	uint32_t size;              //!< max size of heap
	block_t** children;         //!< List of children
	int childnum;               //!< Number of child blocks
};

typedef struct _kheap kheap_t;

// functions for dealing with kheap

// initiates the heap
void
kheap_init();

// allocates a void pointer
void* 
kmalloc(size_t);

// frees a pointer
void
kfree(void*);

#endif // KHEAP_H
