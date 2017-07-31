
#include <paging.h>
#include <stdint.h>
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <kernel.h>
#include <malloc.h>
#include <pmm.h>
#include <isr.h>

uint8_t* temp_mem;
int paging_enable = 0;

uint32_t heap_start;

page_directory_t* kpage_dir;

extern uint8_t* bitmap;
extern uint32_t bitmap_size;

uint32_t end_mem_addr = (uint32_t) &end;

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
        uint32_t start = start_va & 0xfffff000;
        uint32_t end   = end_va   & 0xfffff000;

        while (start <= end){
                if(iden_mpa)
                        allocate_page(dir, start, start / PAGE_SIZE, is_kernel, is_writable);
                else
                        allocate_page(dir, start, 0, is_kernel, is_writable);

                start += PAGE_SIZE;
        }
}

void
allocate_page(page_directory_t* dir, uint32_t virtual_addr, uint32_t frame,
              int is_kernel, int is_writable)
{
        page_table_t* table = NULL;
        if (!dir){
                printf("Allocate page: page directory empty\n");
                return;
        }

        uint32_t page_dir_idx = PAGEDIR_INDEX(virtual_addr);
        uint32_t page_tbl_idx = PAGETBL_INDEX(virtual_addr);

        table = dir->ref_tables[page_dir_idx];
        if (table){
                table = malloc_a(sizeof(page_table_t));
                if (table == NULL){
                        printf("Allcoate page: Table not allocated\n");
                        return;
                }

                memset(table, 0, sizeof(page_table_t));

                uint32_t t = (uint32_t) virtual2phys(kpage_dir, table);

                dir->tables[page_dir_idx].frame     = t >> 12;
                dir->tables[page_dir_idx].present   = 1;
                dir->tables[page_dir_idx].rw        = 1;
                dir->tables[page_dir_idx].user      = 1;
                dir->tables[page_dir_idx].page_size = 0;

                dir->ref_tables[page_dir_idx] = table;

        }

        if (!table->pages[page_tbl_idx].present){
                uint32_t t;

                if (frame)
                        t = frame;
                else
                        t = allocate_block();

                table->pages[page_tbl_idx].frame = t;
                table->pages[page_tbl_idx].present = 1;
                table->pages[page_tbl_idx].rw = 1;
                table->pages[page_tbl_idx].user = 1;
        }
}

void
free_region(page_directory_t* dir, uint32_t start_va, uint32_t end_va, int free)
{
        uint32_t start = start_va & 0xfffff000;
        uint32_t end   = end_va   & 0xfffff000;

        while (start <= end){
                free_page(dir, start, 1);
                start += PAGE_SIZE;
        }
}

void
free_page(page_directory_t* dir, uint32_t virt_addr, int free)
{
        if (dir == TEMP_PAGE_DIRECTORY) return; // can't free origininal

        uint32_t page_dir_idx = PAGEDIR_INDEX(virt_addr);
        uint32_t page_tbl_idx = PAGETBL_INDEX(virt_addr);

        if(!dir->ref_tables[page_dir_idx]){
                printf("free_page: page dir entry does not exist\n");
                return;
        }

        page_table_t* table = dir->ref_tables[page_dir_idx];
        if(!table->pages[page_tbl_idx].present){
                printf("free_page: page tbale entry does not exist\n");
                return;
        }

        if(free)
                free_block(table->pages[page_tbl_idx].frame);

        table->pages[page_tbl_idx].present = 0;
        table->pages[page_tbl_idx].frame   = 0;
}

void
paging_init()
{
        temp_mem = bitmap + bitmap_size;

        kpage_dir = dumb_malloc(sizeof(page_directory_t), 1);
        memset(kpage_dir, 0, sizeof(page_directory_t));

        uint32_t i = end_mem_addr;
        while (i < end_mem_addr + 4 * (1024 * 1024)){
                allocate_page(kpage_dir, i, 0, 1, 1);
                i += PAGE_SIZE;
        }

        i = end_mem_addr + 4 * (1024*1024);
        while (i < end_mem_addr + 4 * (1024*1024) + (48 * (1024*1024))){
                allocate_page(kpage_dir, i, 0, 1, 1);
                i += PAGE_SIZE;
        }

        switch_page_dir(kpage_dir, 0);

        enable_paging();

        allocate_region(kpage_dir, 0x0, 0x10000, 1, 1, 1);
}

void
switch_page_dir(page_directory_t* dir, uint32_t phys)
{
        uint32_t t;
        if (!phys)
                t = (uint32_t) virtual2phys(TEMP_PAGE_DIRECTORY, dir);
        else
                t = (uint32_t) dir;

        asm volatile ("mov %0, %%cr3" :: "r"(t));
}

void
enable_paging()
{
        uint32_t cr0, cr4;

        asm volatile ("mov %%cr4, %0" : "=r"(cr4));
        CLEAR_PSEBIT(cr4);
        asm volatile ("mov %0, %%cr4" :: "r"(cr4));

        asm volatile ("mov %%cr0, %0" : "=r"(cr0));
        SET_PGBIT(cr0);
        asm volatile ("mov %0, %%cr0" :: "r"(cr0));

        paging_enable = 1;
}

void*
ksbrk(int size)
{        return NULL; // TODO
}

void
copy_page_directory(page_directory_t* dest, page_directory_t* src)
{
        for (uint32_t i = 0; i < 1024; i++){
                if(kpage_dir->ref_tables[i] == src->ref_tables[i]){
                        dest->tables[i]     = src->tables[i];
                        dest->ref_tables[i] = src->ref_tables[i];
                } else {
                        dest->ref_tables[i] = copy_page_table(src, dest, i, src->ref_tables[i]);
                        uint32_t phys = (uint32_t) virtual2phys(src, dest->ref_tables[i]);
                        dest->tables[i].frame   = phys >> 12;
                        dest->tables[i].user    = 1;
                        dest->tables[i].rw      = 1;
                        dest->tables[i].present = 1;
                }
        }
}

page_table_t*
copy_page_table(page_directory_t* src_dir, page_directory_t* dest_dir,
                uint32_t page_dir_idx, page_table_t* src_tbl)
{
        page_table_t* table = (page_table_t*) malloc_a(sizeof(page_table_t));

        for (int i = 0; i < 1024; i++){
                if(!table->pages[i].frame)
                        continue;

                uint32_t src_virt_addr = (page_dir_idx << 22) | (i << 12) | 0;
                uint32_t dst_virt_addr = src_virt_addr;
                uint32_t tmp_virt_addr = 0;

                allocate_page(dest_dir, dst_virt_addr, 0, 0, 1);
                allocate_page(src_dir, tmp_virt_addr,
                              (uint32_t) virtual2phys(dest_dir, (void*) dst_virt_addr),
                              0, 1);

                if (src_tbl->pages[i].present)  table->pages[i].present = 1;
                if (src_tbl->pages[i].rw)       table->pages[i].rw = 1;
                if (src_tbl->pages[i].user)     table->pages[i].user = 1;
                if (src_tbl->pages[i].accessed) table->pages[i].accessed = 1;
                if (src_tbl->pages[i].dirty)    table->pages[i].dirty = 1;

                memcpy((void*)tmp_virt_addr, (void*)src_virt_addr, PAGE_SIZE);
        }
}

void
page_fault_handler(struct regs* reg)
{
        return; // TODO
}
