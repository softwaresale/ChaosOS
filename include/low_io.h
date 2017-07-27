#ifndef LOW_IO_H
#define LOW_IO_H 1

#include <stdint.h>

// gets the data from port
uint8_t inb(uint16_t port);

// puts data into port
void outb(uint16_t port, uint8_t data);

// gets a word from a short
uint16_t inw(uint16_t port);

// puts a word into port
void outw(uint16_t port, uint16_t data);

uint32_t inl(uint16_t);

void outl(uint16_t, uint32_t);

#endif
