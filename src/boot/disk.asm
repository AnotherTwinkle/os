; Arguments:
; es:bx : Address where to load
; dl : Target drive
; dh : Amount of sectors to load
; cl : Sector to start loading from (1 indexed)

; Bios interface for disk read
; ah = 0x02 (Disk read function)
; ch = Cylinder (0 indexed)
; dh = Head (0 indexed)
; cl = Sector (1 indexed)
; al = Amount of sectors to read

; On success:
; al = number of sectors read

; On failure:
; cf = set to signal general fault
; al = number of sectors read != requested

disk_load:
    push dx
    mov ah, 0x02   ; Disk read
    mov ch, 0x00   ; Cylinder 0 
    mov al, dh     ; Read dh sectors
    mov cl, cl     ; Already set as argument
    mov dh, 0x00   ; From head 0
    call reg_debug
    int 0x13       ; Disk r/w
    pop dx
    call reg_debug
    jc DISK_ERROR_CF ; Jump if cf is set
    cmp dh, al
    jne DISK_ERROR ; Jump if expected number of sectors weren't read
    ret
    
DISK_ERROR_CF:
    mov bx, DISK_ERROR_MSG_CF
    call print16bit
    call newline
    jmp $          ; Stop code execution
DISK_ERROR_MSG_CF:
    db "DISK ERROR (CF SET)!", 0

DISK_ERROR:
    mov bx, DISK_ERROR_MSG
    call print16bit
    call newline
    jmp $

DISK_ERROR_MSG:
    db "DISK ERROR!", 0


