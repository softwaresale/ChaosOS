
/* This file is used for handling paging */

#ifndef PAGING_H
#define PAGING_H 1

#include <stdint.h>

void
paging_init();

void*
get_physaddr(void*);

void
map_page(void*, void* unsigned int);

#endif // PAGING_H
