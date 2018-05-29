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

/* This is only a test entry point */
void dummy_entry(){
}

static char *
get_name(void);

void kmain()
{
	tty_init(); // initate the terminal interface
  printf("TTY ... [done]\n");

	idt_install();
  printf("IDT ... [done]\n");

	isr_install();
  printf("ISR ... [done]\n");

	irq_install();
  printf("IRQ .. [done]\n");

	keyboard_install();
  printf("Keyboard ... [done]\n");

	paging_init();
  printf("Paging ... [done]\n");

	malloc_init();
  printf("Heap ... [done]\n");

	__asm__ __volatile__ ("sti");

	printf("Core functionality installed\n");
	printf("Welcome to ChaosOS\n");

  char *msg = get_name();
  printf("This is written by: %s\n", msg);
}

static char *
get_name(void)
{
  char *msg = malloc(8);
  msg[0] = 'C';
  msg[1] = 'h';
  msg[2] = 'a';
  msg[3] = 'r';
  msg[4] = 'l';
  msg[5] = 'i';
  msg[6] = 'e';
  msg[7] = '\0';

  return msg;
}
