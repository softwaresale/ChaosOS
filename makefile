CC=i386-elf-gcc
LD=i386-elf-ld
CFLAGS=-m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
       -nostartfiles -nodefaultlibs -Wextra -Werror -c -I./include
LDFLAGS=-T linker.ld -melf_i386
AS=nasm
ASFLAGS=-f elf
SRC=$(wildcard kernel/*.c drivers/*.c)
OBJ=$(SRC:.c=.o)

all: kernel.elf

kernel.elf: $(OBJ) boot/loader.o kernel/ixx.o
	$(LD) $(LDFLAGS) $(OBJ) boot/loader.o -o kernel.elf

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

%.o: %.asm
	nasm $< -f elf -o $@

clean:
	rm $(OBJ) kernel/ixx.o boot/loader.o kernel.elf

run: all
	qemu-system-i386 -kernel kernel.elf
