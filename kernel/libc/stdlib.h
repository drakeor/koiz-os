#ifndef _STDLIB_H_
#define _STDLIB_H_

#include "malloc.h"

#include <stdint.h>


/**
 * panic() - Causes a kernel panic and immediately stops further processing
 * 
 * @message:    panic message to write
 */
void panic(char *message);

/**
 * printf() - Prints a message to the standard display device
 * 
 * @message:    message to write
 */
void printf(char *format, ...);

/**
 * eswap_uint16() - Swaps Endianness of 16-bit unsigned short
 * 
 * @value:  Short to swap endianness of
 */
uint16_t eswap_uint16(uint16_t value);

/**
 * eswap_uint32() - Swaps Endianness of 32-bit unsigned integer
 * 
 * @value:  Integer to swap endianness of
 */
uint32_t eswap_uint32(uint32_t value);

/* Deals with things like processing standard input/output streams */
void stdlib_update(void);

/* Puts a character into the stdio input buffer. Mainly used by the 
keyboard driver. Iunno how to structure it more permanently atm.
TODO: Think of a better architecture. */
void stdlib_put_stdio_input_char(char character);

/* grabs character off the standard input stream */
char stdlib_pop_stdio_input_char(void);

#endif