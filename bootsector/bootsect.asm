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
call print_hex_word
call print_newline

; BIOS puts the boot drive in dl. We'll store it.
mov [boot_drive], dl

; Load 5 sectors from 0x0000(ES):0x9000(BX)
mov bx, 0x9000 
mov dh, 5
mov dl, [boot_drive]
call disk_load

; Read the contents at address 0x9000
mov bx, [0x9000]
call print_hex_word

; Read the contents at address 0x9000 + 512
mov bx, [0x9000 + 512]
call print_hex_word

; Main logic. For now, it's just an infinite loop
main_loop:
    jmp main_loop


; -- End of usual program --
include 'diskload.asm'
include 'printfuncs.asm'

; Variables
startup_message:
    db 'KoiZ OS. Stack Start: ', 0x00

; Buffer out to 512 bytes
times 510-($-$$) db 0
dw 0xaa55 

; We know that BIOS will load only the first 512 - byte sector from the disk ,
; so if we purposely add a few more sectors to our code by repeating some
; familiar numbers , we can prove to ourselfs that we actually loaded those
; additional two sectors from the disk we booted from.
times 256 dw 0xdada
times 256 dw 0xface