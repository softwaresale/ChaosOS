#ifndef LOW_IO_H
#define LOW_IO_H 1

// gets the data from port
unsigned char port_byte_get(unsigned short port);

// puts data into port
void port_byte_put(unsigned short port, unsigned char data);

// gets a word from a short
unsigned short port_word_get(unsigned short port);

// puts a word into port
void port_word_put(unsigned short port, unsigned short data);

#endif
