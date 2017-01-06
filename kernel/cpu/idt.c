#include <idt.h>
#include <low_io.h>
#include <memory.h>

// create an idt with 256 entries
struct idt_entry idt[256];

// create an entry to an idt pointer
struct idt_ptr idtp;

extern void idt_load();

// sets the idt entry
void idt_set_gate(unsigned int num, unsigned long base, unsigned short sel,
                  unsigned char flags) {
	
	// set the high and low bits of the address
	idt[num].base_low_bits  = (unsigned short)(base & 0xFFFF);
	idt[num].base_high_bits = (unsigned short)((base >> 16) & 0xFFFF);
	
	// set the selector
	idt[num].selector       = sel;

	// set always zero to zero
	idt[num].always0        = 0;

	// set any flags
	idt[num].flags          = flags;

}

// installs an idt
void idt_install(){
	
	// set some stuff?
	idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
	
	// set the base
	idtp.base = (unsigned int) &idt;
	
	// zero the idt
	memset(&idt, 0, sizeof(struct idt_entry) * 256);
	
	// load the idt via processor's directive
	idt_load();
}
