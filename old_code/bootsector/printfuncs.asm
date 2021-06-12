; Useful printing functions

; Prints a new line
print_newline:
    push ax
    mov ah, 0x0E
    mov al, 0x0A
    int 10h
    mov al, 0x0D
    int 10h
    pop ax
    ret

; Put the address of the string in si prior to calling. Terminated by 0x00
print_string:
    pusha
    mov ah, 0x0E
    .p_loop:
        lodsb
        cmp al, 0x00
        je .done
        int 10h
        jmp .p_loop
    .done:
        popa
        ret

; Assume the byte is in bx prior to calling
print_hex_word:
    push ax
    mov al, bh
    call print_hex_byte
    mov al, bl
    call print_hex_byte
    pop ax
    ret

; Assume the byte is already in al
print_hex_byte:
    pusha
    ; Get the higher 4 bits
    mov dl, al
    shr al, $4
    call print_hex
    ; Get the lower 4 bits
    mov al, dl
    mov cl, 0x0F
    and al, cl
    call print_hex
    popa
    ret

; Assume the byte is already in al
print_hex:
    pusha
    mov ah, 0x0E
    add al, 0x30
    cmp al, 0x39
    jbe .done
    add al, 0x07
    .done:
        int 10h
        popa
        ret
