#include "display.h"
#include "basic_io.h"

/* Helper functions for translating offsets */
int get_offset(int col, int row) { return (row * MAX_COLS + col) * VCHAR_CELLSIZE; }

int get_offset_row(int offset) { return offset / (VCHAR_CELLSIZE * MAX_COLS); }

int get_offset_col(int offset) { 
    return (offset - (get_offset_row(offset) * MAX_COLS * VCHAR_CELLSIZE)) / VCHAR_CELLSIZE; 
}


/*
 * Grabs the current cursor position from VGA ports
 * Grabs the high byte with (data 14)
 * Grabs the low byte with (data 15)
 */
uint16_t get_cursor_offset()
{
    io_byte_out(REG_SCREEN_CTRL, 14);
    uint16_t cur_offset = io_byte_in(REG_SCREEN_DATA);
    cur_offset <<= 8;

    io_byte_out(REG_SCREEN_CTRL, 15);
    cur_offset |= io_byte_in(REG_SCREEN_DATA);

    return cur_offset * VCHAR_CELLSIZE;
}

/*
 * Sets cursor position
 */

void set_cursor_offset(uint16_t offset)
{
    offset /= 2;
    io_byte_out(REG_SCREEN_CTRL, 14);
    io_byte_out(REG_SCREEN_DATA, (uint8_t)(offset >> 8));
    io_byte_out(REG_SCREEN_CTRL, 15);
    io_byte_out(REG_SCREEN_DATA, (uint8_t)(offset & 0xFF));
}


/* 
 * Prints an ASCII character at a screen position
 * Leave char_color at 0 to do default black on white
 * Returns the new offset which is also the new position of the cursor
 */
int print_char(uint8_t character, uint8_t col, uint8_t row, uint8_t char_color) {

    uint8_t *vidmem = (uint8_t*) VIDEO_ADDRESS;
    
    /* Silently cancel for out of range values. TODO: Change this later */
    if(col >= MAX_COLS || row >= MAX_ROWS)
        return 0;

    if(!char_color)
        char_color = make_display_color(
                        BASE_DISPLAY_COLOR_WHITE, 
                        BASE_DISPLAY_COLOR_BLACK
                    );

    uint16_t vidmem_offset = get_offset(col, row);

    /* Move to next line */
    if(character == '\n') {
        row = get_cursor_offset();
        vidmem_offset = get_offset(0, row+1);
    } else {
        vidmem[vidmem_offset] = character;
        vidmem[vidmem_offset+1] = char_color;
        vidmem_offset += 2; 
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
        
        vidmem_offset -= MAX_COLS * VCHAR_CELLSIZE;
    }

    /* Set new cursor position */
    set_cursor_offset(vidmem_offset);
    return vidmem_offset;

}

void clear_screen()
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

void print(char *message, uint8_t color)
{
    int offset = get_cursor_offset();

    int i=0;
    while(message[i] != 0) {

        int row = get_offset_row(offset);
        int col = get_offset_col(offset);

        /* print each character. print_char will return the cursor position */
        offset = print_char(message[i], col, row, color);

        ++i;
    }
}
