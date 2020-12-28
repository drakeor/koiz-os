#include "stdlib.h"
#include "../config.h"
#include "../drivers/display.h"
#include "../drivers/serial.h"

#include "stdarg.h"

#define DEFAULT_TEXT_COLOR 0x0F
#define DEFAULT_ERROR_COLOR 0x05

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
    printf_impl(format, arg);
    va_end(arg);
}