#ifndef KB_H
#define KB_H 1

#include <kernel.h>

// handler for keyboard
void keyboard_handler(struct regs* r);

// installer
void keyboard_install();

#endif
