#include <malloc.h>
#include <paging.h>
#include <bmheap.h>
#include <stdint.h>
#include <stddef.h>

// declaration of global variable
kheap_t kernel_heap;

void
malloc_init()
{
	extern const unsigned int end;
	unsigned int end_addr = (unsigned int) &end;
	kheap_init(&kernel_heap);
	kheap_add_block(&kernel_heap, end_addr, 0x100000, 16);
}

void*
malloc(uint32_t size)
{
	return kheap_alloc(&kernel_heap, size);
}

void
free(void* ptr)
{
	kheap_free(&kernel_heap, ptr);
}
