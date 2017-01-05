#ifndef ISR_H
#define ISR_H 1

#include <kernel.h>

// installs the isr
void isr_install();

// fault handler
void fault_handler(struct regs* r);

#endif
