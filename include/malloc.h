
/* This file handles malloc and free commands. Also, stores a global heap */

#ifndef MALLOC_H
#define MALLOC_H 1

#include <stdint.h>
#include <stddef.h>
#include <bmheap.h>
#include <paging.h>
#include <string.h>

// this is a global heap variable
extern kheap_t kernel_heap;

// functions

void
malloc_init();

void*
malloc(uint32_t);

void
free(void* ptr);

#endif // MALLOC_H
