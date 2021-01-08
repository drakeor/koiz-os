#include "stdlib.h"
#include "stdarg.h"

#include "../config.h"
#include "../drivers/display.h"
#include "../drivers/serial.h"

#include "../core/interrupt_handler.h"
#include "../core/pic.h"

#define DEFAULT_TEXT_COLOR 0x0F
#define DEFAULT_ERROR_COLOR 0x05

extern void load_idt(void);

void print(char *message)
{
#ifdef USE_COM1_AS_LOG
    write_serial_string(PORT_COM1, message);
#endif 
    kprint(message, DEFAULT_TEXT_COLOR);
}

void error(char *message)
{
#ifdef USE_COM1_AS_LOG
    write_serial_string(PORT_COM1, message);
#endif 
    kprint(message, DEFAULT_ERROR_COLOR);
}

void kernel_init()
{
#ifdef USE_COM1_AS_LOG
    /* Test out our serial port */
    uint8_t serial_started = init_serial(PORT_COM1);
    if(serial_started != 0) {
        print("error initializaing serial port COM1\n");
        return;
    }
    print("using serial COM1 for logging serial...\n");
    //write_serial_string(PORT_COM1, "using serial COM1 for logging\n");
#endif

    /* Setup Load our IDT */
    load_idt();

    /* Run self tests */
    self_test_idt();

    /* Load PIC stuff */
    PIC_remap();
    PIC_set_interrupt_masks();

    printf("kernel init");
}

void clear_display()
{
    kclear_screen();
}

int printf_impl(char *format, va_list arg) 
{
    error("printf not yet implemented");
}

/*
 * Helper function for the printf function
 */
void print_uint_to_string(unsigned int number, unsigned int base)
{
    /* error checking */
    if(base > 16) {
        error("printf base cannot be greater than 16!");
        return;
    }

    /* handle printing different bases */
    if(base == 2)
        kprint("0b", DEFAULT_TEXT_COLOR);
    if(base == 16)
        kprint("0x", DEFAULT_TEXT_COLOR);

    /* if the number is 0, just print 0. save some processing */
    if(number == 0) {
        kprint_char('0', DEFAULT_TEXT_COLOR);
        return;
    }

    /* 
     * otherwise convert the number into a string.
     * it'll be in reverse order so print it in reverse.
     */
    const int buffer_size = 32;
    char buffer[buffer_size];
    int current_pos = 0;
    const char numtochar[] = "0123456789ABCDEF";
    do {
        buffer[current_pos] = numtochar[number % base];
        number /= base;
        ++current_pos;
    } while(number != 0 && current_pos < buffer_size);

    int i;
    for(i = current_pos; i > 0; i--)
        kprint_char(buffer[i-1], DEFAULT_TEXT_COLOR);
}

/*
 * Panic function
 */
__attribute__((__noreturn__))
void panic(char *message)
{
    error("\n");
    error("KERNEL PANIC: ");
    error(message);
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
                kprint_char('%', DEFAULT_TEXT_COLOR);
            } else if(*(format + 1) == 's') {
                char* subString = va_arg(arg, char*);
                kprint(subString, DEFAULT_TEXT_COLOR);
            } else if(*(format + 1) == 'd') {
                unsigned int num = va_arg(arg, unsigned int);
                print_uint_to_string(num, 10);
            } else if(*(format + 1) == 'x') {
                unsigned int num = va_arg(arg, unsigned int);
                print_uint_to_string(num, 16);
            } else if(*(format + 1) == 'b') {
                unsigned int num = va_arg(arg, unsigned int);
                print_uint_to_string(num, 2);
            } else if(*(format + 1) == '\0') {
                error("printf error: next character is null");
                break;
            } else {
                error("printf error: Unknown escape sequence %");
                kprint_char(*(format + 1), DEFAULT_TEXT_COLOR);
                break;
            }
            format++;
        /* Simply print the next character */
        } else {
            kprint_char(*format, DEFAULT_TEXT_COLOR);
        }

        // Move to the next character
        format++;
    }
    va_end(arg);
}