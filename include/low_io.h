#ifndef LOW_IO_H
#define LOW_IO_H 1

// gets the data from port
unsigned char inb(unsigned short port);

// puts data into port
void outb(unsigned short port, unsigned char data);

// gets a word from a short
unsigned short inw(unsigned short port);

// puts a word into port
void outw(unsigned short port, unsigned short data);

#endif
