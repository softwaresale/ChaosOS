#include <kheap.h>
#include <paging.h>
#include <stdint.h>
#include <stddef.h>
#include <tty.h>
#include <string.h>
#include <stdio.h>

// get the end address
extern const unsigned int end;
unsigned int end_addr = (unsigned int) &end; // end address

// create a heap
kheap_t kheap;

void
kheap_init()
{
	kheap.placement_address = end_addr;
	kheap.size = 0x00300000 - end_addr;
	kheap.children = 0; // no children yet
	kheap.childnum = 0;
}

static inline void
kheap_add_child(block_t* block)
{
	kheap.children[kheap.childnum++] = block; // add block?	
}

void
kheap_specs()
{
	char pa[16];
	char size[16];
	
	hex_to_ascii(kheap.placement_address, pa);
	int_to_ascii(kheap.size, size);

	print("Kheap placement address: ");
	puts(pa);
	print("Kheap size: ");
	puts(size);
}

void*
kmalloc(size_t size)
{	
	// first, scan children to look for available block
	int i;
	for (int i = 0; i < kheap.childnum; i++){
		block_t* tmp = kheap.children[i]; // get a child
		if (tmp->used == 1) {
			continue; // block is being used
		} else {
			if (tmp->block_size >= size) {
				// we found a block with enough space
				tmp->used = 1; // set the block to being used
				return (void*) tmp->address; // return address of child
			} else {
				continue; // block is too small
			}
		}
	}
	
	if ((kheap.size - size) == 0){
		return (void*) 0; // return NULL, heap is full
	}

	// if no value found, then we must make a new block
	block_t newBlock;
	newBlock.address = kheap.placement_address; // new address is placed

	kheap.placement_address += (sizeof(block_t) + size); // increment up placement address
	kheap.size -= (sizeof(block_t) + size);              // decrement size
	
	kheap_add_child(&newBlock); // add the block to the heap

	return (void*) newBlock.address;
}

void
kfree(void* ptr)
{
	// get the address of the pointer
	uint32_t addr = (uint32_t)&ptr;

	// iterate through the children
	for (int i = 0; i < kheap.childnum; i++){
		block_t* block = kheap.children[i];
		if (block->address = addr) /* We found it */ {
			block->used = 0; // set it to unused
			int* i = (int*) ptr;
			*i = 0;
			break;
		}
	
	}
}

