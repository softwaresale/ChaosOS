#ifndef IRQ_H
#define IRQ_H 1

#include <kernel.h>
#include <low_io.h>
#include <mem.h>

/* Installs a handler into an irq */
// PARAM irq     -- which irq to install in
// PARAM handler -- function pointer to handler
void irq_install_handler(int irq, void (*handler)(struct regs* r));

/* Uninstalls handler */
// PARAM irq -- which irq to uninstall from
void irq_uninstall_handler(int irq);

/* Remaps the IRQ */
void irq_remap(void);

/* Installs the irq */
void irq_install();

/* Handles error and stuff I think */
void irq_handler(struct regs* r);

#endif
