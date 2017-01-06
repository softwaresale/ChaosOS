#ifndef IDT_H
#define IDT_H 1

#include <memory.h>
#include <low_io.h>

/* Struct mapping idt entry */
struct idt_entry
{
	unsigned short base_low_bits;  // lower address bits of fault handler
	unsigned short selector;       // Kernel segment
	unsigned char  always0;        // always zero. No dip
	unsigned char  flags;          // flags
	unsigned short base_high_bits; // upper address bits
};

/* Idt pointer struct */
struct idt_ptr
{
	unsigned short limit;
	unsigned int   base; 
}__attribute__((packed));

// Sets idt gate
// PARAM num   -- which idt
// PARAM base  -- base address of idt
// PARAM sel   -- selector
// PARAM flags -- flags
void idt_set_gate(unsigned int num, unsigned long base, unsigned short sel,
                  unsigned char flags);

// installs the idt
void idt_install();

#endif
