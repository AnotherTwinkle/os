[org 0x7c00]

jmp bootloader_init

%include "src/boot/disk.asm"
%include "src/boot/gdt.asm"
%include "src/boot/util.asm"

BOOT_DRIVE: db 0
GREETINGS_16BIT: db "Hello, 16 bits!", 0
GREETINGS_PM : db "Welcome to 32-bit land!", 0
MSG_LOADING_KERNEL : db "Loading kernel...", 0

KERNEL_OFFSET equ 0x1000  ; WARNING : make sure kernel is at 0x1000 while building

[bits 16]

bootloader_init:
	; BIOS stores boot drive in dl
	mov [BOOT_DRIVE], dl

	; Setup stack points for any functions we may call in 16 bit
	mov bp, 0x8000
	mov sp, bp

	; Greetings
	mov bx, GREETINGS_16BIT
	call print16bit
	call newline

	mov bx, MSG_LOADING_KERNEL
	call print16bit

	; mov dx, KERNEL_OFFSET
	; call print_hex

	; Enable a20... It is enabled by default on qemu though
	in al, 0x92
	or al, 2
	out 0x92, al

	call load_kernel 

	jmp switch_to_protected_mode  ; we are not returning

; Kernel must be loaded in 16 bit mode using bios routines
load_kernel:
	mov ax, 0x0           ; Make sure es (offset) is 0
	mov es, ax

	mov bx, KERNEL_OFFSET ; Load in KERNEL_OFFSET
	mov dh, 30            ; Load 30 sectors
	mov dl, [BOOT_DRIVE]  ; Target drive

	call disk_load
	ret

switch_to_protected_mode:

	cli 				  ; Clear interrupts
	lgdt [gdt_descriptor] ; Pass gdt table

	mov eax, cr0
	or eax, 0x1           ; First bit = 1 for 32 bit mode
	mov cr0, eax

	jmp CODE_SEG:pm_init  ; Far jump to 32 bits!

[bits 32]
pm_init:

	; Redefine segments
	mov ax, DATA_SEG
	mov ds, ax
	mov ss, ax  
	mov es, ax
	mov fs, ax
	mov gs, ax

	; New stack
	mov ebp, 0x80000
	mov esp, ebp

	jmp main

main:
	call cls
	mov ebx, GREETINGS_PM
	call print

	call cls
	call KERNEL_OFFSET  ; Hand control over to kernel.

	jmp $

end:
	times 510-($ - $$) db 0
	dw 0xaa55