#include <idt.h>
#include <isr.h>
#include <irq.h>
#include <tty.h>
#include <kb.h>

/* This is only a test entry point */
void dummy_entry(){
}

void kmain(){

	init_tty();
	idt_install();
	isr_install();
	irq_install();
	keyboard_install();

	print("Hello World\n");
	print("Welcome to ChaOS\n");
}
