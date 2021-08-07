#include <stdint.h>
#include "keyboard.h"
#include "../io/basic_io.h"
#include "../irq/pic.h"
#include "../../libc/stdlib.h"

/* 
 * Interrupt that the keyboard is assigned to 
 * Note that if you change this here, you'll have to change it in the
 * interrupt_handler.asm as well
 */
#define KEYBOARD_INTERRUPT 0x21

/* Helpful for lower */
#define NULL 0

/* TODO: Switch this to a jump table later.. */
uint8_t scan_to_ascii(uint8_t scan_code) 
{
    const uint8_t scan_to_ascii_lookup[0x100] = {
        /* 0x00 - 00xF */
        NULL, NULL, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', NULL, NULL,
        /* 0x10 - 0x1F */
        'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', 0xA, NULL, 'a', 's',
        /* 0x20 - 0x2F */
        'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', NULL, '\\', 'z', 'x', 'c', 'v',
        /* 0x30 - 0x3F */
        'b', 'n', 'm', ',', '.', '/', NULL, NULL, NULL, ' ', NULL, NULL, NULL, NULL, NULL, NULL,
        /* 0x40 - 0x4F */
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, '7', '8', '9', '-', '4', '5', '6', '+', '1',
        /* 0x50 - 0x5F */
        '2', '3', '0', '.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        /* 0x60 - 0x6F */
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        /* 0x70 - 0x7F */
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        /* 0x80 - 0x8F */
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        /* 0x90 - 0x9F */
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        /* 0xA0 - 0xAF */
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        /* 0xB0 - 0xBF */
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        /* 0xC0 - 0xCF */
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        /* 0xD0 - 0xDF */
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        /* 0xE0 - 0xEF */
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        /* 0xF0 - 0xFF */
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
    };

    return scan_to_ascii_lookup[scan_code];
}


/*
 * Interrupt Handler for the keyboard (0x21)
 * Converts scan codes to characters
 */
void keyboard_interrupt_handler(void) 
{
    /* Grab scan code and acknowledge */
    uint8_t scan_code = io_byte_in(KBD_DATA_PORT);
    pic_acknowledge(KEYBOARD_INTERRUPT);

    uint8_t ascii_char = scan_to_ascii(scan_code);

    /* Place the character in standard input for now */
    if(ascii_char != NULL)
        stdlib_put_stdio_input_char(ascii_char);
}
