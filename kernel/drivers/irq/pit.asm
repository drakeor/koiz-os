format elf
use32

include '../../libc/ccall.inc'

; Reference
; 
;  Command register 
;  (Copy-pasted from https://wiki.osdev.org/Programmable_Interval_Timer)
; 
;   Bits         Usage
;    6 and 7      Select channel :
;                    0 0 = Channel 0
;                    0 1 = Channel 1
;                    1 0 = Channel 2
;                    1 1 = Read-back command (8254 only)
;    4 and 5      Access mode :
;                    0 0 = Latch count value command
;                    0 1 = Access mode: lobyte only
;                    1 0 = Access mode: hibyte only
;                    1 1 = Access mode: lobyte/hibyte
;    1 to 3       Operating mode :
;                    0 0 0 = Mode 0 (interrupt on terminal count)
;                    0 0 1 = Mode 1 (hardware re-triggerable one-shot)
;                    0 1 0 = Mode 2 (rate generator)
;                    0 1 1 = Mode 3 (square wave generator)
;                    1 0 0 = Mode 4 (software triggered strobe)
;                    1 0 1 = Mode 5 (hardware triggered strobe)
;                    1 1 0 = Mode 2 (rate generator, same as 010b)
;                    1 1 1 = Mode 3 (square wave generator, same as 011b)
;     0            BCD/Binary mode: 0 = 16-bit binary, 1 = four-digit BCD


; Constants
pit_io_channel0         = 0x40
pit_io_channel1         = 0x41
pit_io_channel2         = 0x42
pit_io_command_register = 0x43
pit_hz_signal           = 1193182 

section '.text' executable

    ; public functions
    public pit_initialize
    public pit_interrupt_handler

    ; Standard Lib Functions
    extrn printf
    extrn panic

    ; Does things like install the timer 
    pit_initialize:
        push eax
        push ebx
        push ecx
        push edx

        ; Send command byte 0x36 to command register
        mov al, 00110100b
        out pit_io_command_register, al

        ; Send in divisor low and high bytes to Channel 0
        mov ax, [pit_divisor]
        out pit_io_channel0, al
        mov al, ah
        out pit_io_channel0, al

        ; Calculate and store hz. notorious div instruction but w/e.
        ; it's only done once...
        mov edx, 0
        mov eax, pit_hz_signal
        mov cx, [pit_divisor]
        div ecx
        mov [pit_hz], eax

        pop edx
        pop ecx
        pop ebx
        pop eax

        ret

    ; Handles interrupts
    pit_interrupt_handler:
        push eax

        ; Increment the interrupt count
        add [pit_interrupt_count], 1

        ; Handle very-possible overflow situation.
        ; I simply fail-fast here
        jo .pit_overflow_interrupt_count

        ; Send EOI to the PIC
        mov al, 0x20
        out 0x20, al

        pop eax

        ret

    ; Handle interrupt count overflow overflow
    .pit_overflow_interrupt_count:
        ccall panic, pit_panic_msg
        hlt

section '.bss'
    public pit_interrupt_count
    
    pit_success_msg     db "PIT Successfully initialized! Divisor: %x",0,0xA
    pit_panic_msg       db "Interrupt Count Overflow!",0,0xA
    pit_divisor         dw 0xFFFF
    pit_hz              dd 0x0
    pit_interrupt_count dd 0x0
    

section '.data' writeable