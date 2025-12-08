[org 0x8000]

; Important ^ org is set to 0x8000

jmp setup

VBA_MODE_INFO:
	times 256 db 1


%include "src/boot/disk.asm"
%include "src/boot/gdt.asm"
%include "src/boot/util.asm"

BOOT_DRIVE: db 0
DISPLAY_FAIL_MSG : db "VESA failed", 0
GREETINGS_16BIT: db "Hello, 16 bits!", 0

KERNEL_START equ 10   	  ; 8 sectors were loaded for stage 2 bootloader
						  ; So kernel starts at sector 10

KERNEL_OFFSET equ 0xa000 ; WARNING : make sure kernel is at 0x10000 while building

[bits 16]
setup:
	mov [BOOT_DRIVE], dl

	; Greetings
	mov bx, GREETINGS_16BIT
	call print16bit
	call newline

	; mov dx, KERNEL_OFFSET
	; call print_hex

	; Enable a20... It is enabled by default on qemu though
	in al, 0x92
	or al, 2
	out 0x92, al

	call setup_display
	call load_kernel 

	jmp switch_to_protected_mode  ; we are not returning

setup_display:
	mov ah, 0x0
	mov al, 0x13   			; 320*200 256 coors
	int 0x10
	
; Kernel must be loaded in 16 bit mode using bios routines
load_kernel:
	mov dh, 30           	; Load 30 sectors
	mov cl, KERNEL_START  	; Starting from this sector

	mov bx, KERNEL_OFFSET 	; Load into ES:BX
	mov dl, [BOOT_DRIVE]  	; Target drive

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
	call KERNEL_OFFSET  ; Hand control over to kernel.

	jmp $