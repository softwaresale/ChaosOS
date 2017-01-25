#include <stdint.h>
#include <stddef.h>
#include <kheap.h>

// create a base pointer
void* base = (void*) 0x10000;

void* KHEAP_START = (void*) 0x10000;
void* KHEAP_END   = (void*) 0x100000;
void* cur_heap    = (void*) 0x10000;

heap_block_t* extend_block(heap_block_t* last, uint32_t size){

	// create a temporary heap block
	heap_block_t* temp;

	// set temp to current heap pointer
	temp = (heap_block_t*) cur_heap;
	
	// make sure there is room
	if( ((char*)cur_heap + BLOCK_SIZE + size) > (char*)KHEAP_END)
		return NULL; // all memory has been exhausted

	temp->size = size; // set the block's size
	temp->free = 0;    // set block to free
	temp->next = NULL; // there is nothing after it
	temp->prev = last; // set last to before
	if (last)
		last->next = temp; // set last's next to temp
	
	// update current heap pointer
	cur_heap = (void*)((char*)cur_heap + BLOCK_SIZE + size);

	// return block
	return temp;
}

void split_block(heap_block_t* last, uint32_t size){

	// make a new block
	heap_block_t* new_block;

	// if leftover memory is big enough, split
	if( (last->size - size) >= (BLOCK_SIZE + 4) ) {
		
		new_block       = (heap_block_t*)(last->data + size); // instantiate the new block
		new_block->size = last->size - size - BLOCK_SIZE;     // set new block's size
		new_block->free = 1;                                  // block is taken
		new_block->next = last->next;                         // insert block inbetween the two blocks
		new_block->prev = last;

		last->next = new_block; // update last block
		last->size = size;      // update size
	}
}

heap_block_t* allocate_block(heap_block_t* last, uint32_t size){
	
	// create two blocks
	heap_block_t *temp, *prev;
	
	temp = last; // set temp to last
	prev = NULL; // have yet to set this one
	while (temp){
		if (temp->free == 1 && temp->size >= size){
			split_block(temp, size); // split the block
			temp->free = 0;          // set the block to free
			return temp;             // return the newly split block
		}
	}

	return extend_block(prev, size); // extend the heap if no free block large enough is found
}

void* kmalloc(uint32_t size){

	heap_block_t* temp;                // make a temporary heap block
	uint32_t size_align = align(size); // align the size

	if (base)
		temp = allocate_block(base, size_align); // allocate a block
	else {
		temp = extend_block(base, size_align);   // extend heap if nessessary
		base = temp;                             // set base pointer
	}

	if (temp)
		return temp->data; // points to user requested space
	else
		return NULL;       // no block can be found
}

void fuse_block(heap_block_t* last){

	heap_block_t* temp; // create a temporary block

	// fuse the next block if free
	temp = last->next;
	while(temp){
		if (temp->free == 1){
			last->size += BLOCK_SIZE + (last->next)->size; // addend size
			last->next = temp->next;                       // insert block

			temp = last->next;
		} else break;
	}

	// fuse previous block if possible
	temp = last->prev;
	while(temp){
		if (temp->free == 1){
			temp->size += BLOCK_SIZE + last->size; // addend the size
			temp->next = last->next;               // set the positioning
			last = temp;                           // increment

			temp = last->prev;                     // positioning
		} else break;
	}
}

void kfree(void* fr){

	heap_block_t* temp; // create a temporary block
	
	// points to start of meta info
	temp = (heap_block_t*)((char*)fr-BLOCK_SIZE);
	temp->free = 1; // free the block
	
	// fuse if possible
	fuse_block(temp);
}

