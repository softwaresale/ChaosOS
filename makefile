CC=i386-elf-gcc
LD=i386-elf-ld
CFLAGS=-m32 -ffreestanding -Wextra -Werror -std=gnu99 -c -I./include -O2
LDFLAGS=-T linker.ld -melf_i386
AS=nasm
ASFLAGS=-f elf
SRC=$(wildcard kernel/*.c drivers/*.c kernel/cpu/*.c kernel/lib/*.c kernel/memory/*.c)
OBJ=$(SRC:.c=.o)

all: kernel.elf

kernel.elf: $(OBJ) boot/loader.o kernel/cpu/interrupt.o kernel/memory/asmpaging.o
	$(LD) $(LDFLAGS) $(OBJ) boot/loader.o kernel/cpu/interrupt.o kernel/memory/asmpaging.o -o kernel.elf

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

%.o: %.asm
	nasm $< -f elf32 -o $@

clean:
	rm $(OBJ) kernel/cpu/interrupt.o boot/loader.o kernel.elf

run: all
	qemu-system-i386 -kernel kernel.elf
