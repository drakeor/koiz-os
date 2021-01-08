load_kernel:
    ; Load 30 sectors from 0x0000(ES):KERNEL_OFFSET(BX)
    ; Each sector is (8? MB) so make SURE it's large enough!
    ; 8 * 30 = 256? MB
    mov bx, KERNEL_OFFSET 
    mov dh, 30
    mov dl, [boot_drive]
    call disk_load