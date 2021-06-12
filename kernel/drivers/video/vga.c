#include "../io/basic_io.h"
#include "vga.h"

/* Hard-coded display values */
#define VIDEO_ADDRESS 0xb8000
#define MAX_ROWS 25
#define MAX_COLS 80
#define VCHAR_CELLSIZE 2

/* Screen I/O ports */
#define REG_SCREEN_CTRL 0x3D4
#define REG_SCREEN_DATA 0x3D5

/**
 * get_offset() - Translates a col and row into cursor offset
 * 
 * @col:    Column to use
 * @row:    Row to use
 */
uint16_t get_offset(int col, int row) { 
    return (row * MAX_COLS + col) * VCHAR_CELLSIZE; 
}

/**
 * offset_to_row() - Translates a cursor offset into a row
 * 
 * @offset: Cursor offset
 * 
 * Returns the row relating to the cursor offset value
 */
int offset_to_row(int offset) { 
    return offset / (VCHAR_CELLSIZE * MAX_COLS); 
}

/**
 * offset_to_col() - Translates a cursor offset into a column
 * 
 * @offset: Cursor offset
 * 
 * Returns the row relating to the cursor offset value
 */
int offset_to_col(int offset) { 
    return (offset - 
        (offset_to_row(offset) * MAX_COLS * VCHAR_CELLSIZE)
    ) / VCHAR_CELLSIZE; 
}

/** 
 * get_cursor_offset() - Grabs the cursor position from the hardware
 * 
 * Returns the cursor position from hardware
 */
uint16_t get_cursor_offset(void)
{
    io_byte_out(REG_SCREEN_CTRL, 14);
    uint16_t cur_offset = io_byte_in(REG_SCREEN_DATA);
    cur_offset <<= 8;

    io_byte_out(REG_SCREEN_CTRL, 15);
    cur_offset |= io_byte_in(REG_SCREEN_DATA);
    return cur_offset * VCHAR_CELLSIZE;
}

/**
 * set_cursor_offset() - Sets cursor position in the hardware
 * 
 * @offset:            - offset to set the cursor at.
 */

void set_cursor_offset(uint16_t offset)
{
    offset /= VCHAR_CELLSIZE;
    io_byte_out(REG_SCREEN_CTRL, 14);
    io_byte_out(REG_SCREEN_DATA, (uint8_t)(offset >> 8));
    io_byte_out(REG_SCREEN_CTRL, 15);
    io_byte_out(REG_SCREEN_DATA, (uint8_t)(offset & 0xFF));
}


/** 
 * print_char() - internal helper method for printing a character
 *
 * @character:  - character to print
 * @col:        - which column to print character
 * @row:        - which row to print character
 * @char_color  - Leave char_color at 0 to do default black on white
 * 
 * Returns the new offset which is also the new position of the cursor
 */
int 
print_char(uint8_t character, uint8_t col, uint8_t row, uint8_t char_color) {

    uint8_t *vidmem = (uint8_t*) VIDEO_ADDRESS;
    
    /* Silently cancel for out of range values. TODO: Change this later */
    if(col >= MAX_COLS || row >= MAX_ROWS)
        return 0;

    if(!char_color)
        char_color = vga_make_display_color(
                        VGA_DISPLAY_COLOR_WHITE, 
                        VGA_DISPLAY_COLOR_BLACK
                    );

    uint16_t vidmem_offset = get_offset(col, row);

    /* Move to next line */
    if(character == '\n') {
        row = row + 1;
        vidmem_offset = get_offset(0, row);
    } else {
        vidmem[vidmem_offset] = character;
        vidmem[vidmem_offset+1] = char_color;
        vidmem_offset += VCHAR_CELLSIZE; 
    }
    
    /* Handle out of bounds by scrolling */
    if(vidmem_offset >= MAX_ROWS * MAX_COLS * VCHAR_CELLSIZE) {
        int i;
        int j;

        /* Copy next line to the current line in memory */
        for(i = 1; i < MAX_ROWS; i++)
            for(j = 0; j < MAX_COLS * VCHAR_CELLSIZE; j++)
                *(vidmem + get_offset(0, i-1) + j)  
                    = *(vidmem + get_offset(0, i) + j);

        /* Clear out the last line and decrement the offset */
        for(j = 0; j < MAX_COLS * VCHAR_CELLSIZE; j++)
            *(vidmem + get_offset(0, MAX_ROWS-1) + j) = 0;
        
        /* Fix offsets and variables */
        row = row - 1;
        vidmem_offset -= MAX_COLS * VCHAR_CELLSIZE;
    }

    /* Set new cursor position */
    set_cursor_offset(vidmem_offset);
    return vidmem_offset;

}

void vga_clear_screen(void)
{
    uint8_t row = 0;
    uint8_t col = 0;
    for(row = 0; row < MAX_ROWS; row++) {
        for(col = 0; col < MAX_COLS; col++) {
            print_char(' ', col, row, 0);
        }
    }
    set_cursor_offset(0);
}


void vga_print_screen(char *message, uint8_t color)
{
    int offset = get_cursor_offset();

    int i=0;
    while(message[i] != 0x0) {

        int row = offset_to_row(offset);
        int col = offset_to_col(offset);

        /* print each character. print_char will return the new cursor position */
        offset = print_char(message[i], col, row, color);
        
        ++i;
    }
}

void vga_print_screen_char(char character, uint8_t color)
{
    int offset = get_cursor_offset();

    int row = offset_to_row(offset);
    int col = offset_to_col(offset);

    /* print character */
    print_char(character, col, row, color);

}