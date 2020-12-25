#include "stdlib.h"
#include "../config.h"
#include "../drivers/display.h"
#include "../drivers/serial.h"

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