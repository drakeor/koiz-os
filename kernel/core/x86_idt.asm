format elf
use32

; This macro pushes 0 as the error code for interrupts
; that do not have an error code
; It also pushes the interrupt number
macro no_error_code_interrupt_handler int_num* {
    public isr_#int_num
    isr_#int_num#:
        pushd 0
        pushd int_num
        jmp common_interrupt_handler
}

; This macro already assumes that the error code is on the stack
; It only pushes the interrupt number
macro error_code_interrupt_handler int_num* {
    public isr_#int_num
    isr_#int_num#:
        pushd int_num
        jmp common_interrupt_handler
}

include 'ccall.inc'

section '.text' executable

    public load_idt
    public common_interrupt_handler
    extrn printf
    extrn interrupt_handler
    extrn setup_idt

    ; at this point the stack should contain [esp + 4] -> first entry in EDT
    ; [esp] -> the return address    
    load_idt:
        PUSHAD
        ;mov eax, [esp+4]
        ;lidt eax
        
        ;mov eax, 0xDEAD0008
        ;mov [idt_start], eax

        lidt [idt_info]
        call setup_idt

        ccall printf, msg

        int 0x0

        POPAD
        ret

    common_interrupt_handler:
        ; Manually save registers
        ; Only because we want to match the stack that the C function is expecting
        pushd eax
        pushd ebx
        pushd ecx
        pushd edx
        pushd esi
        pushd edi
        pushd ebp

        ; Call our C function
        call interrupt_handler

        ; Restore registers
        popd ebp
        popd edi
        popd esi
        popd edx
        popd ecx
        popd ebx
        popd eax

        ; Restores the ESP
        add esp, 8

        ; We need to use iret to return from 
        iret

    no_error_code_interrupt_handler 0
    no_error_code_interrupt_handler 1

section '.rodata'
    msg db "rawr",0
    public idt_info
    public idt_start
    idt_start:
        irq0:
            ;dw ((interrupt_handler_0 shl 0x30) shr 0x30)
            dw 0xDEAD
            dw 0x0008
            db 0x00
            db 10101110b
            dw 0xDEAD
            ;dw (interrupt_handler_0 shr 0x10)
    idt_end:
    idt_info:
        dw idt_end - idt_start - 1
        dd idt_start

section '.data' writeable

