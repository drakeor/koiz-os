#ifndef _BASIC_IO_H_
#define _BASIC_IO_H_

#include <stdint.h>

/**
 * io_byte_in() - Reads a byte of data from an IO port
 * 
 * @port:   Port to read from
 * 
 * Returns byte of data read from IO port
 */
uint8_t io_byte_in(uint16_t port);

/**
 * io_word_in() - Reads a word of data from an IO port
 * 
 * @port:   Port to read from
 * 
 * Returns a word of data read from IO port
 */
uint16_t io_word_in(uint16_t port);

/**
 * io_byte_out() - Writes a byte of data to the IO port
 * 
 * @port:   Port to write to
 * @data:   Byte of data to write
 */
void io_byte_out(uint16_t port, uint8_t data);

/**
 * io_word_out() - Writes a word of data to the IO port
 * 
 * @port:   Port to write to
 * @data:   Word of data to write
 */
void io_word_out(uint16_t port, uint16_t data);

/**
 * io_wait() - Waits for some time
 */
void io_wait(void);

#endif