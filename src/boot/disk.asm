; Arguments:
; es:bx : Address where to load
; dl : Target drive
; dh : Amount of sectors to load (boot sector will be ignored)
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
    mov cl, 0x02   ; Starting from sector 2
    mov dh, 0x00   ; From head 0
    int 0x13       ; Disk r/w
    pop dx
    jc DISK_ERROR  ; Jump if cf is set
    cmp dh, al
    jne DISK_ERROR ; Jump if expected number of sectors weren't read
    ret
    
DISK_ERROR:
    call reg_debug
    mov bx, DISK_ERROR_MSG
    call print16bit
    call newline
    jmp $          ; Stop code execution
DISK_ERROR_MSG:
    db "DISK ERROR!", 0