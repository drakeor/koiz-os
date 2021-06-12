use16
use_32bpm:
    ; Disable interrups
    cli 

    ; Load our descriptor
    lgdt [gdt_descriptor]

    ; Set the first bit of cr0 to goto protected mode.
    mov eax , cr0 
    or eax , 0x1
    mov cr0 , eax

    ; Execute farjump to the 32 bit code
    ; This apparently flushes the cpu pipeline of pre-fetched 16bit instructions
    jmp CODE_SEG:init_32bpm

use32
init_32bpm:
    ; Remember that our old segments are useless now
    mov ax, DATA_SEG

    ; Point segment registers to what we defined in the GDT
    mov ds, ax
    mov ss, ax 
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Update stack position so its on top of free space
    mov ebp, 0x80000
    mov esp, ebp

    call main_32b

use16