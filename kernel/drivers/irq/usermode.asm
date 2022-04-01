; Say this is a 32-bit elf file
format ELF
use32

section '.text'
    public _enter_usermode

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

		iretd
	a:
		add esp, 4      ; fix stack
        ;int 3
        int 0x33
        ret
