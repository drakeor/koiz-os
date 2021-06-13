#include "stdlib.h"

#include <stdarg.h>

#include "../drivers/video/vga.h"
#include "../drivers/serial/serial.h"

/* Constants to help us */
#define PRINTF_BUFFER_SIZE 32
#define DEFAULT_TEXT_COLOR 0x0F
#define DEFAULT_ERROR_COLOR 0x05

/* Comment out to disable serial port logging */
#define LOG_ALL_MESSAGES_TO_SERIAL

void std_print(char *message)
{
#ifdef LOG_ALL_MESSAGES_TO_SERIAL
    serial_write_string(PORT_COM1, message);
#endif 
    vga_print_screen(message, DEFAULT_TEXT_COLOR);
}

void std_print_char(char message)
{
#ifdef LOG_ALL_MESSAGES_TO_SERIAL
    serial_write(PORT_COM1, message);
#endif 
    vga_print_screen_char(message, DEFAULT_TEXT_COLOR);
}

void std_error(char *message)
{
#ifdef LOG_ALL_MESSAGES_TO_SERIAL
    serial_write_string(PORT_COM1, message);
#endif 
    vga_print_screen(message, DEFAULT_ERROR_COLOR);
}

/*
 * Helper function for the printf function
 */
void print_uint_to_string(unsigned int number, unsigned int base)
{
    /* error checking */
    if(base > 16) {
        std_error("printf base cannot be greater than 16!");
        return;
    }

    /* handle printing different bases */
    if(base == 2)
        std_print("0b");
    if(base == 16)
        std_print("0x");

    /* if the number is 0, just print 0. save some processing */
    if(number == 0) {
        std_print_char('0');
        return;
    }

    /* 
     * otherwise convert the number into a string.
     * it'll be in reverse order so print it in reverse.
     */
    char buffer[PRINTF_BUFFER_SIZE];
    int current_pos = 0;
    const char numtochar[] = "0123456789ABCDEF";
    do {
        buffer[current_pos] = numtochar[number % base];
        number /= base;
        ++current_pos;
    } while(number != 0 && current_pos < PRINTF_BUFFER_SIZE);

    int i;
    for(i = current_pos; i > 0; i--)
        std_print_char(buffer[i-1]);
}


/*
 * Panic function
 */
__attribute__((__noreturn__))
void panic(char *message)
{
    std_error("\n");
    std_error("KERNEL PANIC: ");
    std_error(message);
    while (1) { }
	__builtin_unreachable();
}

/*
 * A light-weight printf function
 * Supports printing integers, hexidecimal, and other strings.
 */
void printf(char *format, ...)
{
    va_list arg;
    int length;

    va_start(arg, format);
    /*printf_impl(format, arg);*/
    while(*format != '\0') {

        /* Handle special characters */
        if(*format == '%') {
            if(*(format + 1) == '%') {
                std_print_char('%');
            } else if(*(format + 1) == 's') {
                char* subString = va_arg(arg, char*);
                std_print(subString);
            } else if(*(format + 1) == 'c') {
                char character_arg = va_arg(arg, int);
                std_print_char(character_arg);
            } else if(*(format + 1) == 'd') {
                unsigned int num = va_arg(arg, uint32_t);
                print_uint_to_string(num, 10);
            } else if(*(format + 1) == 'x') {
                unsigned int num = va_arg(arg, uint32_t);
                print_uint_to_string(num, 16);
            } else if(*(format + 1) == 'b') {
                unsigned int num = va_arg(arg, uint32_t);
                print_uint_to_string(num, 2);
            } else if(*(format + 1) == '\0') {
                std_error("printf error: next character is null");
                break;
            } else {
                std_error("printf error: Unknown escape sequence %");
                std_print_char(*(format + 1));
                break;
            }
            format++;
        /* Simply print the next character */
        } else {
            std_print_char(*format);
        }

        // Move to the next character
        format++;
    }
    va_end(arg);
}