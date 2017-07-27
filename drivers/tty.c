#include <tty.h>
#include <memory.h>
#include <low_io.h>

// terminal info variables
unsigned short* textbuf; // current buffer
int tty_x = 0;           // set position
int tty_y = 0;           // variables

unsigned short get_offset(int x, int y){
	return y * COLUMNS + x;
}

// moves the cursor
void move_cursor(){

	unsigned short index = get_offset(tty_x, tty_y);
	
	// sends command to move cursor
	outb(CTRL_REG, 14);
	outb(DATA_REG, index >> 8);
	outb(CTRL_REG, 15);
	outb(DATA_REG, index);
}

// clears the screen
void cls(){

	unsigned short blank; // blank entry
	unsigned short attr;  // attribute byte
	
	attr  = GREEN_ON_BLACK << 8; // set the attr byte to green on black
	blank = attr | 0x20;         // set the rest of the byte
	
	for (int i = 0; i < ROWS; i++)
		memsetw(textbuf+ i*COLUMNS, blank, COLUMNS); // set each cell to blank

	tty_x = 0;     // zero
	tty_y = 0;     // position
	move_cursor(); // move the cursor to location
}

void
go_back(int space)
{
	tty_x -= space;
}

void
go_foreward(int space)
{
	tty_x += space;
}

// handles scrolling
void scroll(){

	unsigned short temp, blank, attr; // mem offset index, blank byte, attr byte
	
	attr  = GREEN_ON_BLACK << 8; // set the attribute
	blank = attr | 0x20;         // set the entry
	
	if (tty_y >= ROWS){
	
		temp = tty_y - ROWS + 1; // get the offset
		
		memcpy(textbuf, textbuf + temp*COLUMNS, (ROWS - temp) * COLUMNS * 2); // lots of math that I don't under stand
		memsetw(textbuf + (ROWS - temp)*COLUMNS, blank, COLUMNS); // set more data

		tty_y = ROWS -1; // reset Y value
	}
}

void printchar(const char ch){

	unsigned short* where; // location to print
	unsigned short  attr;  // attribute byte
	
	attr = GREEN_ON_BLACK << 8; // set attr
	
	if (ch == 0x08) /* Handle backspace */  {
	
		if (tty_x > 0)
			tty_x--;

		// delete the character
		where  = textbuf + get_offset(tty_x, tty_y); // set where
		*where = attr | 0x20;                        // set the value of where

	}
	
	if (ch == 0x09) /* Handle tab */
		tty_x = (tty_x + 8) & ~(8-1); // set the x to tab

	if (ch == '\r') /* Carriage return? */
		tty_x = 0;

	if (ch == '\n') /* Handle new line */  {
		tty_y++;
		tty_x = 0;
	}

	if (ch >= ' ') /* All other character */ {
		where  = textbuf + get_offset(tty_x, tty_y); // set the position of where
		*where = attr | ch; // set the character
		tty_x++;
	}
	
	if (tty_x >= COLUMNS){
		tty_x = 0;
		tty_y++;
	}
	
	scroll();      // scroll if nessesary
	move_cursor(); // and move cursor

}

void print(const char* str){
	for (int ii = 0; str[ii] != '\0'; ii++)
		printchar(str[ii]); // print each character of the string
}

void init_tty(){
	textbuf = (unsigned short*) VIDEO_MEM; // set the video memory
	cls(); // clear the screen
}

