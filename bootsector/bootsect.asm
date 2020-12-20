; Boot sector
use16

; Makes printing easier
macro plet letter* {
    mov al, letter
    int 0x10
}

; Go to TTY mode
mov ah, 0x0e

plet 'K'
plet 'o'
plet 'i'
plet 'Z'

main_loop:
    jmp main_loop

; Buffer out to 512 bytes
times 510-($-$$) db 0
dw 0xaa55 