#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <stdint.h>

/* Enum for all the base display colors */
enum VGA_DISPLAY_COLOR
{
    VGA_DISPLAY_COLOR_BLACK         = 0,
    VGA_DISPLAY_COLOR_BLUE          = 1,
    VGA_DISPLAY_COLOR_GREEN         = 2,
    VGA_DISPLAY_COLOR_CYAN          = 3,
    VGA_DISPLAY_COLOR_RED           = 4,
    VGA_DISPLAY_COLOR_MAGENTA       = 5,
    VGA_DISPLAY_COLOR_BROWN         = 6,
    VGA_DISPLAY_COLOR_LIGHTGRAY     = 7,
    VGA_DISPLAY_COLOR_DARKGRAY      = 8,
    VGA_DISPLAY_COLOR_LIGHTBLUE     = 9,
    VGA_DISPLAY_COLOR_LIGHTGREEN    = 10,
    VGA_DISPLAY_COLOR_LIGHTCYAN     = 11,
    VGA_DISPLAY_COLOR_LIGHTRED      = 12,
    VGA_DISPLAY_COLOR_PINK          = 13,
    VGA_DISPLAY_COLOR_YELLOW        = 14,
    VGA_DISPLAY_COLOR_WHITE         = 15
};

/** vga_make_display_color() -  Generates a foreground/background color combo
 * 
 * @foreground:     Foreground color (VGA_DISPLAY_COLOR)
 * @background:     Background color (VGA_DISPLAY_COLOR)
 * 
 * Returns a color that you can pass to vga_print_screen
 */
static inline uint8_t vga_make_display_color(uint8_t foreground, 
    uint8_t background) 
{
    return (background << 4) | foreground;
} 

/**
 * vga_print_screen()     - prints a string to the display device
 *
 * @message:    - string to print terminated by \0
 * @color:      - color to print the string denoted by VGA_DISPLAY_COLOR
 */
void vga_print_screen(char *message, uint8_t color);

/**
 * vga_print_screen()     - prints a string to the display device
 *
 * @character:  - character to print to the display
 * @color:      - color to print the string denoted by VGA_DISPLAY_COLOR
 */
void vga_print_screen_char(char character, uint8_t color);

/**
 * vga_clear_screen()  - clears the screen and resets the cursor
 */
void vga_clear_screen(void);

#endif