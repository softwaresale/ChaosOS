
#include <paging.h>
#include <stdint.h>
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <kernel.h>

uint8_t* temp_mem;
int paging_enable = 0;

page_directory_t* kpage_dir;

void*
virtual2phys(page_directory_t* dir, void* virt_addr)
{
        if(!paging_enable)
                return (void*)(virt_addr - 0xC0000000);

        uint32_t page_dir_idx = PAGEDIR_INDEX(virt_addr);
        uint32_t page_tbl_idx = PAGETBL_INDEX(virt_addr);
        uint32_t page_frm_offset = PAGEFRAME_INDEX(virt_addr);

        if (!dir->ref_tables[page_dir_idx]){
                printf("virtual2phys: page dir entry does not exist\n");
                return NULL;
        }

        page_table_t* table = dir->ref_tables[page_dir_idx];
        if(!table->pages[page_tbl_idx].present){
                printf("virtual2phys: age table entry does not exists\n");
                return NULL;
        }

        uint32_t t = table->pages[page_tbl_idx].frame;
        t = (t << 12) + page_frm_offset;
        return (void*) t;
}

void*
dumb_malloc(uint32_t size, int align)
{
        void* ret = temp_mem;

        if(align && !IS_ALIGN(ret))
                ret = (void*) PAGE_ALIGN(ret);

        temp_mem += size;
        return ret;
}

void
allocate_region(page_directory_t* dir, uint32_t start_va, uint32_t end_va,
                int iden_mpa, int is_kernel, int is_writable)
{

}
