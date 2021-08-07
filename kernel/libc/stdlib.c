#include "stdlib.h"

#include <stdarg.h>

#include "../drivers/memory/pmem.h"
#include "../drivers/video/vga.h"
#include "../drivers/serial/serial.h"

/* Constants to help us */
#define PRINTF_BUFFER_SIZE 32
#define DEFAULT_TEXT_COLOR 0x0F
#define DEFAULT_ERROR_COLOR 0x05

/* Comment out to disable serial port logging */
#define LOG_ALL_MESSAGES_TO_SERIAL

/* Structure for our standard buffers. 
   We're using gust a simple ring buffer. */
struct stdio_buffer {
    int is_init;
    void* b_addr;
    int head_ptr;
    int tail_ptr;
    int b_size;
};
typedef struct stdio_buffer stdio_buffer_t;

/* Our global input, output, and error streams */
stdio_buffer_t std_input_buf = {0};
stdio_buffer_t std_error_buf = {0};
stdio_buffer_t std_output_buf = {0};

/* Helper function to initialize buffers */
void io_buffer_place(stdio_buffer_t* in_buff, char* message) 
{
    /* Lazy initialize buffer */
    if(!in_buff->is_init) {
        in_buff->b_addr = pmem_alloc();
        in_buff->b_size = PHYS_BLOCK_SIZE;
        in_buff->is_init = 1;
    }


    /* Go through message and place in buffer */
    int i = 0;
    uint8_t* b_addr = in_buff->b_addr;
    while(message[i] != '\0')
    {
        /* Copy character to buffer */
        b_addr[in_buff->head_ptr] = message[i];
        in_buff->head_ptr = (in_buff->head_ptr + 1) % in_buff->b_size;
        i++;

        /* If we hit our tail_ptr, we looped around and this
           means we overflowed our buffer. For now, we'll throw an error. */
        /* If this is in userspace from a function call, we'd likely switch
           to another process and hopefully consume data */
        /* Likely undo the write too (set head back one) */
        if(in_buff->head_ptr == in_buff->tail_ptr)
            panic("StdLib IO Buffer overflow!");
    }
}

/* Returns 1 if the io_buffer is empty, or 0 if there's data to read */
int io_buffer_empty(stdio_buffer_t* in_buff)
{
    return (in_buff->head_ptr == in_buff->tail_ptr);
}

/* Pops a character off the IO buffer */
/* Returns 0 if there's nothing left to pop */
uint8_t io_buffer_pop(stdio_buffer_t* in_buff)
{
    if(io_buffer_empty(in_buff))
        return 0;

    uint8_t* b_addr = in_buff->b_addr;
    uint8_t popped_char =  b_addr[in_buff->tail_ptr];
    in_buff->tail_ptr = (in_buff->tail_ptr + 1) % in_buff->b_size;

    return popped_char;
}


void std_print(char *message)
{
#ifdef LOG_ALL_MESSAGES_TO_SERIAL
    serial_write_string(PORT_COM1, message);
#endif 
    /* If we have IOStreams, buffer the input. 
        Otherwise print directly to screen */
    if(pmem_isinit()) {
        io_buffer_place(&std_output_buf, message);
    } else {
        vga_print_screen(message, DEFAULT_TEXT_COLOR);
    }
}

void stdlib_update(void)
{
    /* Print everything that is in the buffer */
    uint8_t next_char = io_buffer_pop(&std_output_buf);
    while(next_char != '\0')
    {
        vga_print_screen_char(next_char, DEFAULT_TEXT_COLOR);
        next_char = io_buffer_pop(&std_output_buf);
    }

}

void std_print_char(char message)
{
#ifdef LOG_ALL_MESSAGES_TO_SERIAL
    serial_write(PORT_COM1, message);
#endif 
    /* If we have IOStreams, buffer the input. 
        Otherwise print directly to screen */
    if(pmem_isinit()) {
        char msg_str[2] = { message, 0x0 };
        io_buffer_place(&std_output_buf, msg_str);
    } else {
        vga_print_screen_char(message, DEFAULT_TEXT_COLOR);
    }
}

/* Standard error bypasses the buffer and prints directly to screen */
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

