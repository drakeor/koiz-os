format elf
use32

; This code is pretty brute-force, but readable..

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

; This macro inserts an interrupt entry into the IDT
; Note that the C func setup_idt will populate the func pointers from 0xDEAD
;
; IDT Entry Layout
; Offset (Low)      - Bits 0-15     - Lower part of the interrupt func addr 
; Selector          - Bits 16-31    - CS selector in GDT (We made it 8 in GDT)
; Zeros             - Bits 32-39    - Should all be zero
;
; Gate Type         - Bits 40-43    - 0b1110 for 32-bit interrupt gate
;   Storage Segment   - Bit 44        - 0 for interrupt and trap gates
;   Desc Priv Level   - Bit 45-46     - Call protection
;   Present           - Bit 47        - Is 1 unless interrupt is unused
;
; Offset (High)     - Bit 48-63     - Higher part of the interrupt func addr 
macro irq_interrupt_entry entry_num* {
    public irq#entry_num
    irq#entry_num:
        dw 0xDEAD
        dw 0x0008
        db 0x00
        db 10101110b
        dw 0xDEAD
}

include 'ccall.inc'

section '.text' executable

    public load_idt
    public common_interrupt_handler

    extrn printf
    extrn interrupt_handler
    extrn setup_idt
    extrn panic

    extrn keyboard_int_handler

    ; at this point the stack should contain [esp + 4] -> first entry in EDT
    ; [esp] -> the return address    
    load_idt:
        PUSHAD

        call setup_idt
        lidt [idt_info]

        POPAD
        ret

    common_interrupt_handler:

        ; Disable interrupts
        cli 

        ; Manually save registers
        ; Only because we want to access the other members of the stack in specific positions

        ; Stack layout is as following:
        ; 44 eflags
        ; 40 cs
        ; 36 eip
        ; 32 error code
        ; 28 interrupt num
        pushd eax ;24
        pushd ebx ;20
        pushd ecx ;16
        pushd edx ;12
        pushd esi ;8
        pushd edi ;4
        pushd ebp ;0

        ; EDI = interrupt #
        ; ECX = error
        ; EBX = 1 to print interrupt, 0 = do not print
        mov edi, esp
        add edi, 28
        mov ecx, esp
        add ecx, 32
        mov ebx, 1 ; Set to 0 to disable printing interrupts

        ; Interrupt 1 is recoverable
        mov edi, esp
        add edi, 28
        mov ecx, 1
        cmp [edi], ecx
        je .resume

        ; Interrupt 3 is recoverable
        mov ecx, 3
        cmp [edi], ecx
        je .resume

        ; Interrupt 0xE is page faults and handled by the page fault handler!
        mov ecx, 0xE
        cmp [edi], ecx
        mov ebx, 0         ; Don't print
        je .call_pagefault_handler

        ; Interrupt 0x21 is handled by the keyboard handler!
        mov ecx, 0x21
        cmp [edi], ecx
        mov ebx, 0         ; Don't print
        je .call_keyboard_handler

        ; Otherwise panic! (We don't know how to handle this interrupt)
        ; This function should NEVER return!
        ccall printf, msg, [edi], [ecx]
        ccall panic, panicmsg
        HLT 

    ; Handle keyboard interrupts
    .call_keyboard_handler:
        push ebx
        ccall keyboard_int_handler
        pop ebx
        jmp .resume

    ; Handle page fault interrupts
    ; Not finished yet so halt
    .call_pagefault_handler:
        push ebx
        mov ebx, cr2
        ccall printf, pagefault_msg, ebx
        pop ebx
        HLT
        jmp .resume

    ; Clean up after interrupt
    .resume:
        cmp ebx, 1
        jne .do_not_print
        ccall printf, msg, [edi], [ecx]
    .do_not_print:
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

        ; Re-enable interrupts
        sti

        ; We need to use iret to return from 
        iret

    ; Once I figure out FASMs macro system for loops I'll change this
    no_error_code_interrupt_handler 0   ; Divide by 0 error
    no_error_code_interrupt_handler 1   ; Debug exception
    no_error_code_interrupt_handler 2   ; Non-maskable interrup
    no_error_code_interrupt_handler 3   ; Breakpoint exception 
    no_error_code_interrupt_handler 4   ; Overflow detected exception
    no_error_code_interrupt_handler 5   ; Out of bounds exception
    no_error_code_interrupt_handler 6   ; Invalid opcode
    no_error_code_interrupt_handler 7   ; No coprocessor
    error_code_interrupt_handler 8   ; Double fault
    no_error_code_interrupt_handler 9   ; Coprocessor segment overrun
    error_code_interrupt_handler 10     ; Bad TSS exception
    error_code_interrupt_handler 11     ; Segment not present
    error_code_interrupt_handler 12     ; Stack segment fault
    error_code_interrupt_handler 13     ; General Protection fault
    error_code_interrupt_handler 14     ; Page fault
    no_error_code_interrupt_handler 15  ; Unknown interrupt
    no_error_code_interrupt_handler 16  ; Coprocessor fault
    no_error_code_interrupt_handler 17  ; Alignment check
    no_error_code_interrupt_handler 18  ; Machine check
    no_error_code_interrupt_handler 19  ; Everything from here and below are reserved
    no_error_code_interrupt_handler 20
    no_error_code_interrupt_handler 21
    no_error_code_interrupt_handler 22 
    no_error_code_interrupt_handler 23
    no_error_code_interrupt_handler 24
    no_error_code_interrupt_handler 25
    no_error_code_interrupt_handler 26
    no_error_code_interrupt_handler 27
    no_error_code_interrupt_handler 28
    no_error_code_interrupt_handler 29
    no_error_code_interrupt_handler 30
    no_error_code_interrupt_handler 31
    no_error_code_interrupt_handler 32
    no_error_code_interrupt_handler 33
    no_error_code_interrupt_handler 34
    no_error_code_interrupt_handler 35
    no_error_code_interrupt_handler 36
    no_error_code_interrupt_handler 37
    no_error_code_interrupt_handler 38
    no_error_code_interrupt_handler 39
    no_error_code_interrupt_handler 40
    no_error_code_interrupt_handler 41
    no_error_code_interrupt_handler 42
    no_error_code_interrupt_handler 43
    no_error_code_interrupt_handler 44
    no_error_code_interrupt_handler 45
    no_error_code_interrupt_handler 46
    no_error_code_interrupt_handler 47
    no_error_code_interrupt_handler 48
    no_error_code_interrupt_handler 49
    no_error_code_interrupt_handler 50
    



section '.rodata'
    msg db "Handling Interrupt %x. Error code: %x",0xA,0
    pagefault_msg db "Page fault address: %x",0xA,0
    panicmsg db "Interrupt is non-recoverable!",0xA,0
    public idt_info
    public idt_start
    idt_start:
        irq_interrupt_entry 0
        irq_interrupt_entry 1
        irq_interrupt_entry 2
        irq_interrupt_entry 3
        irq_interrupt_entry 4
        irq_interrupt_entry 5
        irq_interrupt_entry 6
        irq_interrupt_entry 7
        irq_interrupt_entry 8
        irq_interrupt_entry 9
        irq_interrupt_entry 10
        irq_interrupt_entry 11
        irq_interrupt_entry 12
        irq_interrupt_entry 13
        irq_interrupt_entry 14
        irq_interrupt_entry 15
        irq_interrupt_entry 16
        irq_interrupt_entry 17
        irq_interrupt_entry 18
        irq_interrupt_entry 19
        irq_interrupt_entry 20
        irq_interrupt_entry 21
        irq_interrupt_entry 22
        irq_interrupt_entry 23
        irq_interrupt_entry 24
        irq_interrupt_entry 25
        irq_interrupt_entry 26
        irq_interrupt_entry 27
        irq_interrupt_entry 28
        irq_interrupt_entry 29
        irq_interrupt_entry 30
        irq_interrupt_entry 31
        irq_interrupt_entry 32
        irq_interrupt_entry 33
        irq_interrupt_entry 34
        irq_interrupt_entry 35
        irq_interrupt_entry 36
        irq_interrupt_entry 37
        irq_interrupt_entry 38
        irq_interrupt_entry 39
        irq_interrupt_entry 40
        irq_interrupt_entry 41
        irq_interrupt_entry 42
        irq_interrupt_entry 43
        irq_interrupt_entry 44
        irq_interrupt_entry 45
        irq_interrupt_entry 46
        irq_interrupt_entry 47
        irq_interrupt_entry 48
        irq_interrupt_entry 49
        irq_interrupt_entry 50
    idt_end:
    idt_info:
        dw idt_end - idt_start - 1
        dd idt_start

section '.data' writeable

