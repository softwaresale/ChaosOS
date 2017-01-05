CC=i386-elf-gcc
LD=i386-elf-ld
CFLAGS=-m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
       -nostartfiles -nodefaultlibs -Wextra -Werror -c -I./include
LDFLAGS=-T linker.ld -melf_i386
AS=nasm
ASFLAGS=-f elf
SRC=$(wildcard kernel/*.c drivers/*.c kernel/cpu/*.c)
OBJ=$(SRC:.c=.o)

all: kernel.elf

kernel.elf: $(OBJ) boot/loader.o kernel/cpu/interrupt.o
	$(LD) $(LDFLAGS) $(OBJ) boot/loader.o kernel/cpu/interrupt.o -o kernel.elf

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

%.o: %.asm
	nasm $< -f elf32 -o $@

clean:
	rm $(OBJ) kernel/cpu/interrupt.o boot/loader.o kernel.elf

run: all
	qemu-system-i386 -kernel kernel.elf
