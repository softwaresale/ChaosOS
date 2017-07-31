global loader					; the entry point for elf

MAGIC_NUMBER 	  equ 0x1BADB002		; the magic number
FLAGS	     	  equ 0x0 			; multiboot flags
CHECKSUM          equ -MAGIC_NUMBER		; calculate the checksum
KERNEL_STACK_SIZE equ 4096

extern kmain

section .data
align 4096
global TEMP_PAGE_DIRECTORY
TEMP_PAGE_DIRECTORY:
	dd 0x00000083
	times ((0xC0000000 >> 22) - 1) dd 0
	dd 0x00000083
	times(1024 - (0xC0000000 >> 22) - 1) dd 0

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
	mov ecx, (TEMP_PAGE_DIRECTORY - 0xC0000000)
	mov cr3, ecx

	mov ecx, cr4
	or ecx, 0x00000010
	mov cr4, ecx

	mov ecx, cr0
	or ecx, 0x80000000
	mov cr0, ecx

	lea ecx, [higher_half]
	jmp ecx

higher_half:
	mov dword[TEMP_PAGE_DIRECTORY], 0
	invlpg[0]

	mov eax, 0xCAFEBABE			; loads the magic number
	mov esp, kernel_stack + KERNEL_STACK_SIZE
	push ebx

	call kmain

.loop:
	jmp .loop				; infinite loop
