#include <idt.h>
#include <isr.h>
#include <irq.h>
#include <tty.h>
#include <kb.h>
#include <timer.h>
#include <mem.h>

/* This is only a test entry point */
void dummy_entry(){
}

void kmain(){
	init_tty(); // initate the terminal interface
	print("TTY intiated\n");

	print("Installing IDT...\n");
	idt_install();
	print("IDT installed\n");
	
	print("Installing ISR...\n");
	isr_install();
	print("ISR installed\n");
	
	print("Installing IRQ...\n");
	irq_install();
	print("IRQ installed\n");
	
	print("Installing keyboard...\n");
	keyboard_install();
	print("Keyboard installed\n");

	__asm__ __volatile__ ("sti");

	print("Core functionality installed.\n");
	print("Welcome to ChaosOS\n");
	if (strcmp("Hello World", "Hello World") == 0){
		char* string = "Hello World";
		reverse(string);
		print(string);
	}
}
