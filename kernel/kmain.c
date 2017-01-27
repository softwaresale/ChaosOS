#include <idt.h>
#include <isr.h>
#include <irq.h>
#include <tty.h>
#include <kb.h>
#include <timer.h>
#include <memory.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

/* This is only a test entry point */
void dummy_entry(){
}

void kmain(){
	init_tty(); // initate the terminal interface
	print("TTY intiated\n");

	puts("Installing IDT...");
	idt_install();
	puts("IDT installed");
	
	puts("Installing ISR...");
	isr_install();
	puts("ISR installed");
	
	puts("Installing IRQ...");
	irq_install();
	puts("IRQ installed");
	
	puts("Installing keyboard...");
	keyboard_install();
	puts("Keyboard installed");

	__asm__ __volatile__ ("sti");

	puts("Core functionality installed");
	puts("Welcome to ChaosOS");


}
