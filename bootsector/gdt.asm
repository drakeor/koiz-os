; GDT
; Alright, time for some fun
gdt_start:

; This is required
; 32 bits
gdt_null:
    dd 0x0
    dd 0x0

; Code segment descriptor
; base = 0x0
; max  = 0xFFFFF
gdt_code:
    ; Limit (bits 0-15)
    dw 0xFFFF
    ; Base (bits 0-15)
    dw 0x0
    ; Base (bits 16-23)
    db 0x0
    ; Access Byte
    ; Present           = 1b. Set to 1 for valid selectors.
    ; Priviledge        = 00b. Ring 0 thru 3.
    ; Descriptor Type   = 1b. Should be set for code/data segments and cleared for system segments
    ; Executable bit    = 1b. Is 1 if code in the segment can be executed
    ; Direction bit     = 0b. Code in this segment can be executed from an equal or lower privledge
    ; RW bit            = 1b. Whether read access is allowed for code or write access is allowed for data.
    ; Accessed bit      = 0b. CPU sets to one when the segment is accessed.
    db 10011010b
    ; Flags and limit
    ; Granularity bit   = 1b. 0 means byte granul, 1 means limit is 4 KiB blocks (page granul)
    ; Size bit          = 1b. 0 means 16bit protected mode, 1 means 32 bit protected mode.
    ; Empty bits        = 00b. Padding
    ; Limit bits        = 1111b. 
    db 11001111b
    ; Base
    db 0x0

gdt_data:
    ; Limit (bits 0-15)
    dw 0xFFFF
    ; Base (bits 0-15)
    dw 0x0
    ; Base (bits 16-23)
    db 0x0
    ; Access Byte
    ; Present           = 1b. Set to 1 for valid selectors.
    ; Priviledge        = 00b. Ring 0 thru 3.
    ; Descriptor Type   = 1b. Should be set for code/data segments and cleared for system segments
    ; Executable bit    = 0b. Is 1 if code in the segment can be executed
    ; Direction bit     = 0b. Code in this segment can be executed from an equal or lower privledge
    ; RW bit            = 1b. Whether read access is allowed for code or write access is allowed for data.
    ; Accessed bit      = 0b. CPU sets to one when the segment is accessed.
    db 10010010b
    ; Flags and limit
    ; Granularity bit   = 1b. 0 means byte granul, 1 means limit is 4 KiB blocks (page granul)
    ; Size bit          = 1b. 0 means 16bit protected mode, 1 means 32 bit protected mode.
    ; Empty bits        = 00b. Padding
    ; Limit bits        = 1111b. 
    db 11001111b
    ; Base
    db 0x0

; Label at the end helps the assembler calculate the size of the GDT for the GDT descriptor
gdt_end:

; Our descriptor contains
; - the size of our GDT (1 less than actual size)
; - start address of GDT
gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

; Constants to help us later if needed
CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start
