#include "keyboard.h"
#include "../core/pic.h"
#include "basic_io.h"
#include "display.h"
#include "tty.h"

#define KEYBOARD_INTERRUPT 0x21

/* Bunch of constants to define the keys */
#define KB_KEY_A 0x1E
#define KB_KEY_B 0x30
#define KB_KEY_C 0x2E
#define KB_KEY_D 0x20
#define KB_KEY_E 0x12
#define KB_KEY_F 0x21
#define KB_KEY_G 0x22
#define KB_KEY_H 0x23
#define KB_KEY_I 0x17
#define KB_KEY_J 0x24
#define KB_KEY_K 0x25
#define KB_KEY_L 0x26
#define KB_KEY_M 0x32
#define KB_KEY_N 0x31
#define KB_KEY_O 0x18
#define KB_KEY_P 0x19
#define KB_KEY_Q 0x10
#define KB_KEY_R 0x13
#define KB_KEY_S 0x1F
#define KB_KEY_T 0x14
#define KB_KEY_U 0x16
#define KB_KEY_V 0x2F
#define KB_KEY_W 0x11
#define KB_KEY_X 0x2D
#define KB_KEY_Y 0x15
#define KB_KEY_Z 0x2C

#define KB_KEY_0 0x0B
#define KB_KEY_1 0x02
#define KB_KEY_2 0x03
#define KB_KEY_3 0x04
#define KB_KEY_4 0x05
#define KB_KEY_5 0x06
#define KB_KEY_6 0x07
#define KB_KEY_7 0x08
#define KB_KEY_8 0x09
#define KB_KEY_9 0x0A

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
void keyboard_int_handler(void) 
{
    /* Grab scan code and acknowledge */
    uint8_t scan_code = io_byte_in(KBD_DATA_PORT);
    PIC_acknowledge(KEYBOARD_INTERRUPT);

    uint8_t ascii_char = scan_to_ascii(scan_code);

    /* Simply print the character to the screen for now */
    if(ascii_char != NULL)
        tty_putchar(ascii_char);
}

