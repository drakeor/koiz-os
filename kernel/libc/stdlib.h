#ifndef _STDLIB_H_
#define _STDLIB_H_

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

#endif