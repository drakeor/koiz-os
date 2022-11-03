; System calls are based on
; https://0xax.gitbooks.io/linux-insides/content/SysCall/linux-syscall-1.html

; Call print instruction (System call 2)
mov edi, msg
mov eax, 1
int 0x33

; Call program exit function (System call 1) with error code 0
mov edi, 0
mov eax,2
int 0x33

msg:
    db "Hello from test program!", 0xA, 0x0