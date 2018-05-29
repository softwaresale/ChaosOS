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
	for (unsigned int ii = 0; ii < 1024; ii++)
		page_table[ii] = (ii * 0x1000) | 3;

	// add the page table to the page directory
	page_directory[0] = ((unsigned int)page_table) | 3;

	loadPageDirectory((unsigned int*)page_directory);
	enablePaging();

}

void*
get_physaddr(void* virtualaddr)
{
	// get directory and table index
	unsigned long pdindex = (unsigned long) virtualaddr >> 22;
	unsigned long ptindex = (unsigned long) virtualaddr >> 22 & 0x03FF;

	// get both page table and directory
	unsigned long* pd = (unsigned long*) 0xFFFFF000;
	unsigned long* pt = ((unsigned long*)0xFFC00000) + (0x400 * pdindex);

	return (void*) ((pt[ptindex] & ~0xFFF) + ((unsigned long)virtualaddr & 0xFFF));
}

void
map_page(void* physaddr, void* virtualaddr, unsigned int flags)
{

	// get directory and table index
  unsigned long pdindex = (unsigned long) virtualaddr >> 22;
  unsigned long ptindex = (unsigned long) virtualaddr >> 22 & 0x03FF;

  // get both page table and directory
  unsigned long* pd = (unsigned long*) 0xFFFFF000;
  unsigned long* pt = ((unsigned long*)0xFFC00000) + (0x400 * pdindex);

	pt[ptindex] = ((unsigned long) physaddr) | (flags & 0xFFF) | 0x01; // present
}

