format ELF
use32

section '.text' executable
    extrn main
    call main

    jmp $