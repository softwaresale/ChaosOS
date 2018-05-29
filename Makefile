CC=i686-elf-gcc
LD=i686-elf-ld
AS=i686-elf-as
CFLAGS=-m32 -ffreestanding -Wextra -std=gnu99 -c -I./include -O2
LDFLAGS=-T linker.ld -melf_i386
#LDFLAGS=-Ttext 0x1000 --oformat binary
SRC=$(wildcard kernel/*.c drivers/*.c kernel/cpu/*.c kernel/lib/*.c \
    kernel/memory/*.c kernel/arch/*.c kernel/arch/cpu/*.c)
ASMSRC=$(wildcard boot/*.asm kernel/arch/cpu/*.asm kernel/memory/*.asm)
ASSRC=$(wildcard kernel/arch/*.s)
OBJ=$(SRC:.c=.o)
ASMOBJ=$(ASMSRC:.asm=.o)
ASOBJ=$(ASSRC:.s=.o)

all: kernel.elf

kernel.elf: $(OBJ) $(ASMOBJ) $(ASOBJ) 
	$(LD) $(LDFLAGS) $(OBJ) $(ASMOBJ) $(ASOBJ) -o kernel.elf

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

%.o: %.asm
	nasm $< -f elf32 -o $@

%.o: %.s
	$(AS) $< -o $@

clean:
	rm $(OBJ) $(ASMOBJ) kernel.elf

iso: kernel.elf
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

run: chaos.iso
	qemu-system-i386 -cdrom chaos.iso
