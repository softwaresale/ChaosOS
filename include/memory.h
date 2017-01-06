#ifndef MEMORY_H
#define MEMORY_H 1

#include <stdint.h>
#include <stddef.h>

// Copies memory from src to dest
// PARAM dest  -- destination pointer of value
// PARAM src   -- what to be copied to destination
// PARAM count -- ?
void* memcpy(void* dest, const void* src, int count);

// Set a pointer with a specific value
// PARAM dest  -- pointer to what is to be set
// PARAM val   -- value to set in pointer
// PARAM count -- ?
void* memset(void* dest, char val, int count);

// Same as before, but with 16 bits
unsigned short* memsetw(unsigned short* dest, unsigned short val, int count);

// Allocates memory
// PARAM size -- size of 
//uint32_t malloc(size_t size, int align, uint32_t *phy_addr);
// TODO: UNCOMMENT IF HEAP DOESNT WORK

#endif
