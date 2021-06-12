; Functions for loading the disk

; Loads dh sectors to ES:BX,. Adapted from os-dev book.
disk_load:
    pusha
    push dx 

    ; 0x2 means read sector
    mov ah, 0x02
    mov al, dh

    ; Select head and cylinder 0, and read from the 2nd sector.
    ; Remember 1 is our boot sector
    mov ch, 0x00
    mov dh, 0x00
    mov cl, 0x02

    ; BIOS interrupt
    int 0x13

    ; Error happens if carry flag is set
    jc .disk_error

    pop dx
    cmp dh, al
    jne .disk_error

    popa
    ret

    .disk_error:
        mov si, disk_error_message
        call print_string
        jmp $

disk_error_message:
    db 'Error reading disk!', 0x00
boot_drive:
    db 0