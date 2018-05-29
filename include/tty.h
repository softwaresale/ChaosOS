#ifndef TTY_H
#define TTY_H 1

// terminal constants
#define VIDEO_MEM      0xB8000
#define ROWS           25
#define COLUMNS        80

// color constants
#define WHITE_ON_BLACK 0x0f
#define RED_ON_BLACK   0x04
#define GREEN_ON_BLACK 0x02

// register constants
#define CTRL_REG       0x3d4
#define DATA_REG       0x3d5

// functions
void tty_cls();
void tty_printchar(const char ch);
void tty_print(const char *str);
void tty_init();

#endif
