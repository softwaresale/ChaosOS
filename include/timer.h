#ifndef TIMER_H
#define TIMER_H 1

#include <kernel.h>

/* Timer Driver
 * This is a colletion of funtions used for
 * determining time on the system
 */

/* This function waits until the given ticks has been reached
 * PARAM ticks -- how many ticks until finished
 */
void timer_wait(int ticks);

/* I have no idea what this one does */
void timer_phase(int hz);

/* Handles the timer
 * Increment the "timer ticks" every time the timer fires
 * */
void timer_handler(struct regs* r);

/* Installs the timer into the irq */
void timer_install();

#endif
