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

; Variables
message:
    db 'KoiZ OS', 0x00


; Buffer out to 512 bytes
times 510-($-$$) db 0
dw 0xaa55 