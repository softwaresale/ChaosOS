#include <paging.h>
#include <stdint.h>
#include <stddef.h>

// create a blank page directory with 1024 entries and aligned to 4096
uint32_t page_directory[1024] __attribute__((aligned(4096)));

// create a page table with 1024 entires and aligned to 4096
uint32_t page_table[1024] __attribute__((aligned(4096)));

// assembly functions for enabling paging
extern void loadPageDirectory(uint32_t*);
extern void enablePaging();

void paging_init(){

	// set values of page directory
	for (int ii = 0; ii < 1024; ii++){
	
		// set following flags to the pages
		//   Supervisor:    Only kernel-mode can access
		//   Write enabled: Both readable and writable
		//   Not Present:   Page table is not present
		page_directory[ii] = 0x00000002;
	}

	// fill in page table entries
	unsigned int ii;

	// map 4 mb
	for (ii = 0; ii < 1024; ii++){
		// As the address is page aligned, it will always leave 12 bits zeroed
		// Those bits are used by the attributes
		page_table[ii] = (ii * 0x1000) | 3; // attributes: supervisor, read/write, present
	}

	// add page table into the page directory
	// attributes: supervisor, r/w, present
	page_directory[0] = ((unsigned int) page_table) | 3;

	// now, load the directory and enable paging
	loadPageDirectory(page_directory);
	enablePaging();

}
