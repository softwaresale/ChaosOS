global loader					; the entry point for elf

MAGIC_NUMBER 	  equ 0x1BADB002		; the magic number
FLAGS	     	  equ 0x0 			; multiboot flags
CHECKSUM          equ -MAGIC_NUMBER		; calculate the checksum
KERNEL_STACK_SIZE equ 4096

extern kmain

section .bss
align 4
kernel_stack:
	resb KERNEL_STACK_SIZE

section .multiboot 				; start of code section
align 4						; align 4 bytes
	dd MAGIC_NUMBER				; write the magic number to machine code
	dd FLAGS
	dd CHECKSUM

loader:
	mov eax, 0xCAFEBABE			; loads the magic number	
	mov esp, kernel_stack + KERNEL_STACK_SIZE
	push ebx
	call kmain

.loop:
	jmp .loop				; infinite loop
