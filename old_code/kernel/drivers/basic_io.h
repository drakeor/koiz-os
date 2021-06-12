#ifndef _BASIC_IO_H_
#define _BASIC_IO_H_

#include <stdint.h>

/*
 * Writes data to an IO port
 */
uint8_t io_byte_in(uint16_t port);
uint16_t io_word_in(uint16_t port);

/*
 * Reads data from an IO port
 */
void io_byte_out(uint16_t port, uint8_t data);
void io_word_out(uint16_t port, uint16_t data);

/* Waits an an I/O operation to complete */
void io_wait(void);


#endif