#ifndef _STDLIB_H_
#define _STDLIB_H_

#include "malloc.h"

#include <stdint.h>

/* Constants to help us */
#define PRINTF_BUFFER_SIZE 32
#define DEFAULT_TEXT_COLOR 0x0F
#define DEFAULT_ERROR_COLOR 0x05

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

/*
 * stdlib_flushstdio() - Flushes the output buffer
 *
 * This forces a flush of the output buffer
 */
 void stdlib_flushstdio(void);

/**
 * stdlib_update() - Deals with processing each frame
 * 
 * This handles things like processing the stdio streams
 */
void stdlib_update(void);

/**
 * stdlib_put_stdio_input_char() - Puts a character into the input buffer
 * 
 * @character:  Character to put in buffer
 * 
 * This is mainly used by the keyboard driver to insert characters
 * into standard input. It's sorta hacky, but I don't know a better
 * way to structure it right now.
 * 
 * TODO: Think of a better way to structure this.
 */
void stdlib_put_stdio_input_char(char character);

/**
 * stdlib_pop_stdio_input_char() - Pops a character off the input buffer
 * 
 * Returns 0 if there is nothing to pop off the standard input buffer
 * TODO: Think of a better way to structure this.
 */
char stdlib_pop_stdio_input_char(void);

/**
 * rand() - Generates a random number between 0 and max based off of seed
 * 
 * @seed: Seed to use
 * @max: max value in range
 * 
 * Note that this is the K&R implementation. It's quick and dirty
 */
uint32_t maxrand(int seed,int max);

#endif