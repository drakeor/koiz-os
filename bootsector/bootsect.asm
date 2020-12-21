; Boot sector
use16

; Set global memory offset
org 0x7c00

; Set up the stack
mov bp, 0x8000
mov sp, bp

; Print name of Bootloader
mov si, startup_message
call print_string

; Print out stack location
mov bx, bp
mov al, bh
call print_hex_byte
mov al, bl
call print_hex_byte

; Print newline
mov ah, 0x0E
mov al, 0x0A
int 10h
mov al, 0x0D
int 10h

; BIOS puts the boot drive in dl. We'll store it.
mov [boot_drive], dl

; Load 5 sectors from 0x0000(ES):0x9000(BX)
; mov es, $0
mov bx, 0x9000 
mov dh, 5
mov dl, [boot_drive]
call disk_load

; Print the read contents of bx 
mov bx, [0x9000]
mov al, bh
call print_hex_byte
mov al, bl
call print_hex_byte

; Print the read contents of bx 
mov bx, [0x9000 + 512]
mov al, bh
call print_hex_byte
mov al, bl
call print_hex_byte

; Main logic
main_loop:
    jmp main_loop

; -- End of usual program --

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

; Put the address of the string in si prior to calling. Terminated by 0x00
print_string:
    pusha
    mov ah, 0x0E
    .p_loop:
        lodsb
        cmp al, 0x00
        je .done
        int 10h
        jmp .p_loop
    .done:
        popa
        ret

; Assume the byte is already in al
print_hex_byte:
    pusha
    ; Get the higher 4 bits
    mov dl, al
    shr al, $4
    call print_hex
    ; Get the lower 4 bits
    mov al, dl
    mov cl, 0x0F
    and al, cl
    call print_hex
    popa
    ret

; Assume the byte is already in al
print_hex:
    pusha
    mov ah, 0x0E
    add al, 0x30
    cmp al, 0x39
    jbe .done
    add al, 0x07
    .done:
        int 10h
        popa
        ret

; Variables
startup_message:
    db 'KoiZ OS. Stack Start: ', 0x00
disk_error_message:
    db 'Error reading disk!', 0x00
boot_drive:
    db 0

; Buffer out to 512 bytes
times 510-($-$$) db 0
dw 0xaa55 

; We know that BIOS will load only the first 512 - byte sector from the disk ,
; so if we purposely add a few more sectors to our code by repeating some
; familiar numbers , we can prove to ourselfs that we actually loaded those
; additional two sectors from the disk we booted from.
times 256 dw 0xdada
times 256 dw 0xface