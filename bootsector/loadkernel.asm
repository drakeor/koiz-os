load_kernel:
    ; Load 30 sectors from 0x0000(ES):KERNEL_OFFSET(BX)
    ; Each sector is (512 bytes since it's a floppy) so make SURE it's large enough!
    ; 512bytes * 45 = 23Kb
    mov bx, KERNEL_OFFSET 
    mov dh, 45
    mov dl, [boot_drive]
    call disk_load