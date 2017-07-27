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
#include <paging.h>
#include <malloc.h>
#include <pci.h>

/* This is only a test entry point */
void dummy_entry(){
}

void kmain()
{
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

	puts("Installing timer...");
	timer_install();
	puts("Timer installed...");

	puts("Initiating paging...");
	paging_init();
	puts("Paging initilized");
	
	puts("Initilizing the heap...");
	malloc_init();
	puts("Heap and memory allocation initilized");

	__asm__ __volatile__ ("sti");

	puts("Installing PCI...");
	pci_init();

	puts("Core functionality installed");
	puts("Welcome to ChaosOS");
}
