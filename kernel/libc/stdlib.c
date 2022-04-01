#include "stdlib.h"
#include "stdio.h"

#include <stdarg.h>

#include "../drivers/memory/pmem.h"
#include "../drivers/video/vga.h"
#include "../drivers/serial/serial.h"


/* Comment out to disable serial port logging */
#define LOG_ALL_MESSAGES_TO_SERIAL

/* Our global input and output streams */
stdio_buffer_t std_input_buf = {0};

stdio_buffer_t std_output_buf = { .autoflush_to_screen = 1 };


void stdlib_put_stdio_input_char(char character)
{
    /* Only add it in if we have IOStreams (pmem is avail) */
    /* This lazy-initializes the stream if not exists */
    /* otherwise ignore it */
    if(pmem_isinit() && !std_output_buf.is_initializing) {
        char msg_str[2] = { character, 0x0 };
        io_buffer_place(&std_input_buf, msg_str);
    }
}

char stdlib_pop_stdio_input_char()
{
    /* Only add it in if we have IOStreams (pmem is avail) */
    /* otherwise ignore it */
    if(pmem_isinit() && std_input_buf.is_init) {
        return io_buffer_pop(&std_input_buf);
    }
    return 0;
}

void stdlib_flushstdio(void)
{
    io_buffer_flush_to_screen(&std_output_buf);
}

void stdlib_update(void)
{
    /* Print everything that is in the output buffer */
    stdlib_flushstdio();

    /* For the input buffer, we just print it for now */
    /*next_char = io_buffer_pop(&std_input_buf);
    while(next_char != '\0')
    {
        vga_print_screen_char(next_char, DEFAULT_TEXT_COLOR);
        next_char = io_buffer_pop(&std_input_buf);
    }*/
}

static void std_print(char *message)
{
#ifdef LOG_ALL_MESSAGES_TO_SERIAL
    serial_write_string(PORT_COM1, message);
#endif 
    /* If we have IOStreams, buffer the input. 
        Otherwise print directly to screen */
    if(pmem_isinit() && !std_output_buf.is_initializing) {
        io_buffer_place(&std_output_buf, message);
    } else {
        vga_print_screen(message, DEFAULT_TEXT_COLOR);
    }
}

static void std_print_char(char message)
{
#ifdef LOG_ALL_MESSAGES_TO_SERIAL
    serial_write(PORT_COM1, message);
#endif 
    /* If we have IOStreams, buffer the input. 
        Otherwise print directly to screen */
    /* Don't print is the buffer is still initializing. This
            will lead to an infinite loop */
    if(pmem_isinit() && !std_output_buf.is_initializing) {
        char msg_str[2] = { message, 0x0 };
        io_buffer_place(&std_output_buf, msg_str);
    } else {
        vga_print_screen_char(message, DEFAULT_TEXT_COLOR);
    }
}

/* Standard error bypasses the buffer and prints directly to screen */
static void std_error(char *message)
{
#ifdef LOG_ALL_MESSAGES_TO_SERIAL
    serial_write_string(PORT_COM1, message);
#endif 
    vga_print_screen(message, DEFAULT_ERROR_COLOR);
}

/*
 * Helper function for the printf function
 */
static void print_uint_to_string(unsigned int number, unsigned int base)
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
 * Panic function.
 * Prints directly to the screen and avoids buffers
 */
__attribute__((__noreturn__))
void panic(char *message)
{
    vga_print_screen("\n", DEFAULT_ERROR_COLOR);
    vga_print_screen("KERNEL PANIC: ", DEFAULT_ERROR_COLOR);
    vga_print_screen(message, DEFAULT_ERROR_COLOR);
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


uint16_t eswap_uint16(uint16_t value)
{
    return (value << 8) | (value >> 8 );
}

uint32_t eswap_uint32(uint32_t value)
{
    return ((value<<24)&0xff000000) | 
            ((value<<8)&0xff0000) | 
            ((value>>8)&0xff00) | 
            ((value>>24)&0xff);
}

static uint32_t random_seed = 0;
uint32_t maxrand(int seed,int max)
{
	random_seed = random_seed + seed * 1103515245 + 12345;
	uint32_t rand_num = (unsigned int)(random_seed / 65536) % (max+1);
    return rand_num;
}