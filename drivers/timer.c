#include <timer.h>
#include <irq.h>
#include <kernel.h>
#include <low_io.h>
#include <tty.h>

// keeps track of how many ticks the system has been running for
volatile unsigned int timer_ticks = 0;

// see header file for info
void timer_wait(int ticks){

	unsigned long eticks;         // number of ticks

	eticks = timer_ticks + ticks; // set the size of eticks
	while(timer_ticks < eticks);  // infinte loop
}

void timer_phase(int hz){

	int divisor = 1193180 / hz;          // calculate divisor
	port_byte_put(0x43, 0x36);           // Set command byte to 0x36
	port_byte_put(0x40, divisor & 0xFF); // set low byte of divisor
	port_byte_put(0x40, divisor >> 8);   // set high byte of divisor

}

void timer_handler(struct regs* r){

	// increment timer ticks
	timer_ticks++;

	// extery 18 clocks (about 1 sec), we will display a message on the screen
	if (timer_ticks % 18 == 0)
		print("One second has passed\n");

}

void timer_install(){
	
	irq_install_handler(0, timer_handler); // install timer handler into irq0
}



