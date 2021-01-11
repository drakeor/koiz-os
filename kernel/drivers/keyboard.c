#include "keyboard.h"
#include "../core/pic.h"
#include "basic_io.h"
#include "display.h"

#define KEYBOARD_INTERRUPT 0x21

/* TODO: Switch this to a jump table later.. */
uint8_t scan_to_ascii(uint8_t scan_code) 
{
    uint8_t ascii_char = 0x00;

    /* handle 1 through 9 */
    if(scan_code >= 0x02 && scan_code <= 0x0A) {
        return 0x31 + (scan_code - 0x02);
    }
    /* 
    switch(ascii_char) {
        case 0x02:
            return '1';
        case 0x03:
            return '2';
        case 0x04:
            return '3';
        default:
    }
    return ascii_char; */
    return scan_code;
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
    kprint_char(ascii_char, 0xF);
}

