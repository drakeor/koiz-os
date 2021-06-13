format elf
use32

include '../../libc/ccall.inc'

section '.text' executable

    public vmem_enable_page_dir
    extrn printf

    ;; vmem_enable_page_dir() - Enable the page directory
    ;;
    ;; @ECX:    Address to the page directory to load
    ;; Since we're using fastcall in gcc, we expect the page 
    ;; directory to be in ecx. See
    ;; https://gcc.gnu.org/onlinedocs/gcc-4.3.2//gcc/Function-Attributes.html#Function-Attributes
    vmem_enable_page_dir:
        PUSHAD

        ; Move our first argument to CR3 (page register)
        mov eax, ecx
        mov cr3, eax
        
        ; Print the page directory address
        ccall printf, msg, ecx
        
        ; Toggle paging
        mov eax, cr0
        or eax, 0x80000001
        mov cr0, eax

        POPAD
        ret

section '.rodata'
    msg db "Page Directory Address %x",0xA,0