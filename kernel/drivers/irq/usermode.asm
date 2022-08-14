; Say this is a 32-bit elf file
format ELF
use32

include '../../libc/ccall.inc'

section '.text'
    public _enter_usermode
    extrn set_kernel_stack

    _enter_usermode:
        ; Clear interrupts
        cli

        ; user mode data selector is 0x20 (GDT entry 3). Also sets RPL to 3
        mov ax, 0x23
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax

        push 0x23		; SS, notice it uses same selector as above
		push esp		; ESP
		pushfd			; EFLAGS
		push 0x1b		; CS, user mode code selector is 0x18.
                        ; With RPL 3 this is 0x1b
		lea eax, [a]	; EIP first
		push eax

                
        ; Save the kernel stack
        ; Calling an interrupt in user mode will mutate ESP because of the TSS I think.
        ; It seems to set to the entry in the ESP0 entry in the TSS at least...
        mov eax, esp
        ccall set_kernel_stack, esp

		iretd
	a:
        ; Fix stack
		add esp, 4 

        ; TODO: Set stack to NOT the kernel stack and to the stack allocated to this user process!

        ;int 3
        int 0x33
        ret
