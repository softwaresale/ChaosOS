#include <stdio.h>
#include <tty.h>

void puts(const char* str){
	print(str);
	print("\n");
}
