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
#include <bmheap.h>

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
	
	puts("Initiating paging...");
	paging_init();
	puts("Paging initilized");
	
	puts("Initilizing the heap...");
	kheap_t heap;
	kheap_init(&heap);
	puts("Heap initilized");

	__asm__ __volatile__ ("sti");

	puts("Core functionality installed");
	puts("Welcome to ChaosOS");
	
	const extern unsigned int end;
	unsigned int end_addr = (unsigned int) &end;

	kheap_add_block(&heap, (uintptr_t) end_addr, 0x100000, 16);

	int* ptr = (int*) kheap_alloc(&heap, sizeof(int));

	char addr[16];
	hex_to_ascii(*ptr, addr);

	print("Ptr located at: ");
	puts(addr);

	kheap_free((void*) ptr);
}
