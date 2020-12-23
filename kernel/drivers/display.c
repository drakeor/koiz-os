#include "display.h"

static int currentCursorCol = 0;
static int currentCursorRow = 0;

void print_char(uint8_t character, uint8_t col, uint8_t row, uint8_t char_color) {

    uint8_t *vidmem = (uint8_t*) VIDEO_ADDRESS;
    
    // Silently out of range values
    // TODO: Throw exception
    if(col >= MAX_COLS || row >= MAX_ROWS)
        return;

    if(!char_color)
        char_color = make_display_color(BASE_DISPLAY_COLOR_WHITE, BASE_DISPLAY_COLOR_BLACK);

    uintptr_t vidmem_offset = ((row * MAX_COLS) + col) * 2;
    
    vidmem[vidmem_offset] = character;
    vidmem[vidmem_offset+1] = char_color;
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

    currentCursorCol = 0;
    currentCursorRow = 0;
}

void print(char *message, uint8_t color)
{
    int i=0;
    while(message[i] != 0) {

        // Print the message
        print_char(message[i], currentCursorCol, currentCursorRow, color);

        // Increment the cursor
        ++currentCursorCol;

        // Wrap around to a new line if we're at the end of the current line
        if(currentCursorCol >= MAX_COLS) {
            currentCursorCol = 0;
            ++currentCursorRow;
        }

        // Wrap around for now
        // Implement page scrolling later
        if(currentCursorRow >= MAX_ROWS) {
            currentCursorRow = 0;
        }

        ++i;
    }
}