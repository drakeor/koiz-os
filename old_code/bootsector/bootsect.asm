; Boot sector
use16

; Set global memory offset
; This is the MBR which is loaded by the BIOS at physical address 0x7C00
; Therefore, all our our operations need to be offset by that amount.
org 0x7c00

; Memory offset where we'll load the kernel
KERNEL_OFFSET equ 0x1000

; BIOS puts the boot drive in dl. We'll store it.
mov [boot_drive], dl

; Set up the stack
mov bp, 0x9000
mov sp, bp

; Print name of Bootloader
mov si, startup_message
call print_string

; Print out stack location
mov bx, bp
call print_hex_word
call print_newline

; Detect memory
call do_e820

; Load kernel to disk
call load_kernel

; Move to 32 bit protected mode
call use_32bpm

; Shouldn't get here...
jmp $


; Include 16-bit things
include 'diskload.asm'
include 'printfuncs.asm'
include 'gdt.asm'
include 'loadkernel.asm'
include 'detectmem.asm'

; Include 32-bit things
include 'protmode.asm'
include 'printfuncs32.asm'

; Variables
startup_message:
    db 'KoiZ OS. Stack Start: ', 0x00
bit32_mode_message:
    db 'Entered 32-bit protected mode', 0x00

; 32 bit main func
; Main logic. For now, it's just an infinite loop
use32
main_32b:
    mov ebx, bit32_mode_message
    call print_string_pm

    ; Alright, enter into our kernel!
    call KERNEL_OFFSET

    jmp $

; Buffer bootsector out to 512 bytes
times 510-($-$$) db 0
dw 0xaa55 