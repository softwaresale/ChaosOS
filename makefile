CC=i686-elf-gcc
LD=i686-elf-ld
#LD=i686-elf-gcc
CFLAGS=-m32 -ffreestanding -Wextra -Werror -std=gnu99 -c -I./include -O2
LDFLAGS=-T linker.ld -melf_i386
#LDFLAGS=-Ttext 0x1000 --oformat binary
AS=nasm
ASFLAGS=-f elf
SRC=$(wildcard kernel/*.c drivers/*.c kernel/cpu/*.c kernel/lib/*.c \
    kernel/memory/*.c kernel/arch/*.c kernel/arch/cpu/*.c)
ASMSRC=$(wildcard boot/*.asm kernel/arch/cpu/*.asm kernel/memory/*.asm)
OBJ=$(SRC:.c=.o)
ASMOBJ=$(ASMSRC:.asm=.o)

all: kernel.elf

kernel.elf: $(OBJ) $(ASMOBJ) 
	$(LD) $(LDFLAGS) $(OBJ) $(ASMOBJ) -o kernel.elf

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

%.o: %.asm
	nasm $< -f elf32 -o $@

clean:
	rm $(OBJ) $(ASMOBJ) kernel.elf

iso: all
	cp kernel.elf boot
	cp -r boot iso
	genisoimage -R \
		-b boot/grub/stage2_eltorito \
		-no-emul-boot \
		-boot-load-size 4 \
		-A os \
		-input-charset utf8 \
		-quiet \
		-boot-info-table \
		-o chaos.iso \
		iso

run-iso:
	qemu-system-i386 -cdrom chaos.iso
