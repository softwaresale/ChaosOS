#include <low_io.h>

/* Defining functions. See low-io.h for func description */

unsigned char port_byte_get(unsigned short port){

    // handy little assembly wrapper

    unsigned char result;
    __asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result;
}

void port_byte_put(unsigned short port, unsigned char data){
    __asm__("out %%al, %%dx" : : "a" (data), "d" (port));
}

unsigned short port_word_get(unsigned short port){

    unsigned short result;
	__asm__("in %%dx, %%ax" : "=a" (result) : "d" (port));
	return result;

}

void port_word_put(unsigned short port, unsigned short data){
    __asm__("out %%ax, %%dx" : :"a" (data), "d" (port));
}
