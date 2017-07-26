#ifndef LOW_IO_H
#define LOW_IO_H 1

#include <stdint.h>

// gets the data from port
uint8_t inb(uint16_t);

// puts data into port
void outb(uint16_t, uint8_t);

// gets a word from a short
uint16_t inw(uint16_t);

// puts a word into port
void outw(uint16_t, uint16_t);

uint32_t inl(uint16_t);

void outl(uint16_t, uint32_t);

void insl(uint16_t, unsigned int, unsigned int);

#endif
