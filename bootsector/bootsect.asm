; Boot sector
use16

; Set global memory offset
org 0x7c00

; Set up the stack
mov bp, 0x8000
mov sp, bp

; Print name of BOIS
mov si, message
call print_string

; Print out stack location
mov bx, bp
mov al, bh
call print_hex_byte
mov al, bl
call print_hex_byte

; Main logic
main_loop:
    jmp main_loop

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
message:
    db 'KoiZ OS. Stack Start: ', 0x00


; Buffer out to 512 bytes
times 510-($-$$) db 0
dw 0xaa55 