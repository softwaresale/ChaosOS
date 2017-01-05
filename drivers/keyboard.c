#include <kernel.h>
#include <irq.h>
#include <tty.h>
#include <low_io.h>
#include <memory.h>
#include <idt.h>

/* Keyboard layout standard */

// fucntion key
unsigned int function_key;

unsigned char kbdus[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',     /* 9 */
  '9', '0', '-', '=', '\b',     /* Backspace */
  '\t',                 /* Tab */
  'q', 'w', 'e', 'r',   /* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', /* Enter key */
    0,                  /* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',     /* 39 */
 '\'', '`',   0,                /* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',                    /* 49 */
  'm', ',', '.', '/',   0,                              /* Right shift */
  '*',
    0,  /* Alt */
  ' ',  /* Space bar */
    0,  /* Caps lock */
    0,  /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
  '-',
    0,  /* Left Arrow */
    0,
    0,  /* Right Arrow */
  '+',
    0,  /* 79 - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */
};

/* Function key keyboard layout1 */

unsigned char kbdus_functkey[128] =
{
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*',     /* 9 */
  '(', ')', '_', '+', '\b',     /* Backspace */
  '\t',                 /* Tab */
  'Q', 'W', 'E', 'R',   /* 19 */
  'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', /* Enter key */
    0,                  /* 29   - Control */
  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',     /* 39 */
 '\"', '~',   0,                /* Left shift */
 '|', 'Z', 'X', 'C', 'V', 'B', 'N',                    /* 49 */
  'M', '<', '>', '?',   0,                              /* Right shift */
  '*',
    0,  /* Alt */
  ' ',  /* Space bar */
    0,  /* Caps lock */
    0,  /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
  '-',
    0,  /* Left Arrow */
    0,
    0,  /* Right Arrow */
  '+',
    0,  /* 79 - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */
};

// Handle keyboard interupt
void keyboard_handler(struct regs* r){

	unsigned char scancode; // what is read, or what will be rather

	// read the scancode
	scancode = port_byte_get(0x60);
	
	// if the top bit is set, then it has been released
	if (scancode & 0x80){
	
		// left shift is 0xAA, right is 0xB6
		if (!(scancode ^ 0xAA) || !(scancode ^ 0xB6)){
			if (function_key == 1)
				function_key = 0;
			else
				function_key = 1;
		}
	
	} else {
		
		// 0x3A is the caps lock key
		if (!(scancode ^ 0x3A)){
			if (function_key == 1)
				function_key = 0;
			else
				function_key = 1;
		}
		
		// left shift -> 0x2A ; right shift -> 0x36
		if (!(scancode ^ 0x2A) || !(scancode ^ 0x36)){
			if (function_key == 1)
				function_key = 0;
			else
				function_key = 1;
		}

		if (function_key == 1)
			printchar(kbdus_functkey[scancode]); // print the char
		else
			printchar(kbdus[scancode]); // print the char

	}
	
	// TODO: SET UP ENTER KEY FOR COMMAND PROMPT

}

void keyboard_install(){

	irq_install_handler(1, keyboard_handler);

}

