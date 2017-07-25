#include <low_io.h>
#include <stdint.h>

/* Defining functions. See low-io.h for func description */

uint8_t inline inb(uint16_t port){

    // handy little assembly wrapper

    uint8_t result;
    asm volatile ("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result;
}

void inline outb(uint16_t port, uint8_t data){
    asm volatile ("out %%al, %%dx" : : "a" (data), "d" (port));
}

uint16_t inline inw(uint16_t port){

    uint16_t result;
	asm volatile ("in %%dx, %%ax" : "=a" (result) : "d" (port));
	return result;

}

void inline outw(uint16_t port, uint16_t data){
    asm volatile ("out %%ax, %%dx" : :"a" (data), "d" (port));
}

uint32_t inline inl(uint16_t port){
    uint32_t ret;
    asm volatile ("in %%dx, %%al" : "=a" (ret) : "d" (port));
    return ret;
}

void inline outl(uint16_t port, uint32_t data){
    asm volatile ("out %%al, %%dx" : : "a"(data), "d"(port));
}

void inline insl(uint16_t port, unsigned int buffer, unsigned int count)
{
    asm volatile ("cld; rep; insl" :: "D" (buffer), "d" (port), "c" (count));
}
