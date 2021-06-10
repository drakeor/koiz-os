#include "stdlib.h"
#include "stdarg.h"

#include "../config.h"
#include "../drivers/display.h"
#include "../drivers/serial.h"
#include "../drivers/tty.h"
#include "../drivers/fs_backend/ramdisk.h"

#include "../core/interrupt_handler.h"
#include "../core/pic.h"
#include "../core/mem.h"
#include "../core/vmem.h"

#define DEFAULT_TEXT_COLOR 0x0F
#define DEFAULT_ERROR_COLOR 0x05

extern void load_idt(void);

void std_print(char *message)
{
#ifdef USE_COM1_AS_LOG
    write_serial_string(PORT_COM1, message);
#endif 
    kprint(message, DEFAULT_TEXT_COLOR);
}

void std_print_char(char message)
{
#ifdef USE_COM1_AS_LOG
    write_serial(PORT_COM1, message);
#endif 
    kprint_char(message, DEFAULT_TEXT_COLOR);
}

void std_error(char *message)
{
#ifdef USE_COM1_AS_LOG
    write_serial_string(PORT_COM1, message);
#endif 
    kprint(message, DEFAULT_ERROR_COLOR);
}

void kernel_init(void)
{
#ifdef USE_COM1_AS_LOG
    /* Test out our serial port */
    uint8_t serial_started = init_serial(PORT_COM1);
    if(serial_started != 0) {
        std_print("error initializaing serial port COM1\n");
        panic("cannot initialize serial connection");
        return;
    }
    std_print("using serial COM1 for logging serial...\n");
    //write_serial_string(PORT_COM1, "using serial COM1 for logging\n");
#endif

    /* Setup Load our IDT */
    load_idt();

    /* Run self tests */
    self_test_idt();

    /* Load PIC stuff */
    PIC_remap();
    PIC_set_interrupt_masks();

    /* initialize memory stuff */
    initialize_memory();
    vmem_init();

    /* initialize the ramdisk */
    ramdisk_tests();
    ramdisk_init(PHYS_BLOCK_SIZE * 10);

    //read_kb_scan_code();
    printf("kernel init\n");

    /* Initialize the terminal last */
    init_tty();
}

void kernel_update(void)
{
    tty_update();
}

void clear_display(void)
{
    kclear_screen();
}

int printf_impl(char *format, va_list arg) 
{
    std_error("printf not yet implemented");
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