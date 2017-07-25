#ifndef LOW_IO_H
#define LOW_IO_H 1

#include <stdint.h>

// gets the data from port
uint8_t inline inb(uint16_t);

// puts data into port
void inline outb(uint16_t, uint8_t);

// gets a word from a short
uint16_t inline inw(uint16_t);

// puts a word into port
void inline outw(uint16_t, uint16_t);

uint32_t inline inl(uint16_t);

void inline outl(uint16_t, uint32_t);

void inline insl(uint16_t, unsigned int, unsigned int);

#endif
