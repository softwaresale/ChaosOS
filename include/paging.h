
#ifndef PAGING_H
#define PAGING_H

#include <kernel.h>
#include <stdint.h>
#include <isr.h>

#define PAGE_SIZE 4096
#define IS_ALIGN(addr) ((((uint32_t)(addr)) | 0xFFFFF000) == 0)
#define PAGE_ALIGN(addr) ((((uint32_t)(addr)) & 0xFFFFF000) + 0x1000)

// define some address calc macros
#define PAGEDIR_INDEX(vaddr) (((uint32_t)vaddr) >> 22)
#define PAGETBL_INDEX(vaddr) ((((uint32_t)vaddr) >> 12) & 0x3ff)
#define PAGEFRAME_INDEX(vaddr) (((uint32_t)vaddr) & 0xfff)

// page register manipulation
#define SET_PGBIT(cr) (cr = cr | 0x80000000)
#define CLEAR_PSEBIT(cr) (cr = cr & 0xffffffef)

// Errors
#define ERR_PRESENT   0x1
#define ERR_RW        0x2
#define ERR_USER      0x4
#define ERR_RESERVED  0x8
#define ERR_INST      0x10

typedef struct _page_dir_entry
{

        unsigned int present        : 1;
        unsigned int rw             : 1;
        unsigned int user           : 1;
        unsigned int w_through      : 1;
        unsigned int cache          : 1;
        unsigned int access         : 1;
        unsigned int reserved       : 1;
        unsigned int page_size      : 1;
        unsigned int global         : 1;
        unsigned int available      : 3;
        unsigned int frame          : 20;

} page_dir_entry_t;

typedef struct _page_table_entry
{

        unsigned int present    : 1;
        unsigned int rw         : 1;
        unsigned int user       : 1;
        unsigned int reserved   : 2;
        unsigned int accessed   : 1;
        unsigned int dirty      : 1;
        unsigned int reserved2  : 2;
        unsigned int available  : 3;
        unsigned int frame      : 20;

} page_table_entry_t;

typedef struct _page_table
{

        page_table_entry_t pages[1024];

} page_table_t;

typedef struct _page_directory
{
        // actual entries
        page_dir_entry_t tables[1024];

        // virtual address table
        page_table_t*    ref_tables[1024];

} page_directory_t;

extern page_directory_t* TEMP_PAGE_DIRECTORY;

extern page_directory_t* kpage_dir;

extern uint32_t end;

void* virtual2phys(page_directory_t*, void*);

void* dumb_malloc(uint32_t, int);

void allocate_region(page_directory_t*, uint32_t, uint32_t, int, int, int);

void allocate_page(page_directory_t*, uint32_t, uint32_t, int, int);

void free_region(page_directory_t*, uint32_t, uint32_t, int);

void free_page(page_directory_t*, uint32_t, int);

void paging_init();

void switch_page_dir(page_directory_t*, uint32_t);

void enable_paging();

void* ksbrk(int);

void copy_page_dir(page_directory_t*, page_directory_t*);

page_table_t* copy_page_table(page_directory_t*, page_directory_t*, uint32_t, page_table_t*);

void page_fault_handler(struct regs*);

#endif // PAGING_H
