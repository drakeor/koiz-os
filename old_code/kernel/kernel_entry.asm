format ELF
use32

section '.text' executable
    ;extrn load_idt
    ;call load_idt
    
    extrn main
    call main

    jmp $