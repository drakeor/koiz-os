#include "stdlib.h"
#include "../config.h"
#include "../drivers/display.h"
#include "../drivers/serial.h"

#include "../core/interrupt_handler.h"

#include "stdarg.h"

#define DEFAULT_TEXT_COLOR 0x0F
#define DEFAULT_ERROR_COLOR 0x05

extern void load_idt(void);

void print(char *message)
{
#if USE_COM1_AS_LOG == 1
    write_serial_string(PORT_COM1, message);
#endif 
    kprint(message, DEFAULT_TEXT_COLOR);
}

void error(char *message)
{
#if USE_COM1_AS_LOG == 1
    write_serial_string(PORT_COM1, message);
#endif 
    kprint(message, DEFAULT_ERROR_COLOR);
}

void kernel_init()
{
#if USE_COM1_AS_LOG == 1
    /* Test out our serial port */
    uint8_t serial_started = init_serial(PORT_COM1);
    if(serial_started != 0) {
        print("error initializaing serial port COM1\n");
        return;
    }
    print("using serial COM1 for logging serial...\n");
    //write_serial_string(PORT_COM1, "using serial COM1 for logging\n");
#endif

    /* Load our IDT */
    setup_idt();
    load_idt();
}

void clear_display()
{
    kclear_screen();
}

int printf_impl(char *format, va_list arg) 
{
    error("printf not yet implemented");
}

void printf(char *format, ...)
{
    va_list arg;
    int length;

    va_start(arg, format);
    /*printf_impl(format, arg);*/
    while(*format != '\0') {

        // Handle special characters
        if(*format == '%') {
            if(*(format + 1) == '%') {
                
            } else {
                error("Unknown escape sequence %");
                print(*(format+1));
            }
        }

        // Move to the next character
        format++;
    }
    va_end(arg);
}