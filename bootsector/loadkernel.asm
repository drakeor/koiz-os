load_kernel:
    ; Load 15 sectors from 0x0000(ES):KERNEL_OFFSET(BX)
    mov bx, KERNEL_OFFSET 
    mov dh, 15
    mov dl, [boot_drive]
    call disk_load