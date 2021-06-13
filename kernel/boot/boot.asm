; FASM converted Boot Script off of https://wiki.osdev.org/Bare_Bones
; which was originally written in GNU Assembler
; Multiboot header specification from:
; https://www.gnu.org/software/grub/manual/multiboot/multiboot.html

; Say this is a 32-bit elf file
format ELF
use32

; Magic value that lets bootloader find header
magic_value = 0x1BADB002 

; Bit 0 - Align module on page boundary
; Bit 1 - Provide memory map
; Bit 2 - Make information about video mode table availiable 
; ...
flags = 00000011b

; Multiboot header that marks program as a kernel
section '.multiboot'
    ; Align at 32-bit boundary
    align 4
    ; Magic value that lets bootloader find header
    dd magic_value
    ; Flags
    dd flags
    ; Checksum (MagicValue + Flags) - Checksum = 0
    dd -(magic_value + flags)

; Multiboot standard does not define a value for the Stack pointer
; register (esp) and it is up to the kernel to provide the stack.
; The following will allocate 16KiB of data for a stack and create a
; symbol on top. The stack grows DOWN on x86 and must be 16-byte aligned
; according to the System V ABI standard. The compiler already assumes
; the stack is properly aligned.
section '.bss' align 16
    stack_bottom:
    rb 16384
    stack_top:

; The linker script we built in linker.ld specifies _start as the entry
; point to the kernel. The bootloader jumps to this once the kernel is loaded.
; We shouldn't return from this function as the bootloader is gone 
; at this point.
section '.text'
    extrn kernel_main
    extrn _init_gdt
    extrn kernel_memory_end

    public _start
    _start:
        ; At this point, the bootloader has us in 32-bit protected mode
        ; Interrupts are disabled.
        ; Paging is disabled
        ; Processor state is multiboot standard
        
        ; The first thing we want to do is set up the stack.
        mov [stack_top], esp

        ; Store the EAX, EBX onto the stack.
        ; They contain the multiboot info and magic number correspondingly
        push kernel_memory_end
        push eax
        push ebx

        ; Initialize gdt
        call _init_gdt

        ; Finally, enter the high-level kernel
        ; The stack should only contain EAX and EBX from the start now
        call kernel_main

        ; Put the system into an infinite loop
        cli
        jmp $