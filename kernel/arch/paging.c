#include <paging.h>
#include <stdint.h>

uint32_t page_directory[1024] __attribute__((aligned(4096)));
uint32_t page_table[1024] __attribute__((aligned(4096)));

extern void 
loadPageDirectory(unsigned int*);

extern void 
enablePaging();

void
paging_init()
{
	
	// set all page directory entrys
	for (int i = 0; i < 1024; i++)
		page_directory[i] = 0x00000002;
	
	// set up page table
	for (unsigned int ii = 0; i < 1024; i++)
		page_table[ii] = (ii * 0x1000) | 3;
	
	// add the page table to the page directory
	page_directory[0] = ((unsigned int)page_table) | 3;
	
	loadPageDirectory(page_directory);
	enablePaging();

}
