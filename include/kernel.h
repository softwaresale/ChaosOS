#ifndef KERNEL_H
#define KERNEL_H 1

/* Defines what the stack looks like after running isr */
struct regs {

	unsigned int gs, fs, es, ds;                         // pused the segs last
	unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax; // pused by pusha
	unsigned int int_no, err_code;                       // push byte number
	unsigned int eip, cs, eflags, useresp, ss;           // pushed by processor automatically

};

#endif
