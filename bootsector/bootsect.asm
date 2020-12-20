; Boot sector
use16
main_loop:
    jmp main_loop

; Buffer out to 512 bytes
times 510-($-$$) db 0
dw 0xaa55 