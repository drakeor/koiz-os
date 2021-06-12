#ifndef _SERIAL_H_
#define _SERIAL_H_

#include "stdint.h"

/* 
 * The first two COM ports are almost always the same on x86 systems 
 * We're only going to deal with two of them.
 */
#define PORT_COM1 0x3F8
#define PORT_COM2 0x2F8

/*
 * Error codes
 */
#define SERIAL_SUCCESS                  0
#define SERIAL_ERROR_CONNECTION_FAILED  1
#define SERIAL_ERROR_NO_CONNECTION      2
#define SERIAL_ERROR_ALREADY_CONNECTED  3

/**
 * serial_init() - Starts a serial connection on the port specified
 * 
 * @port:   Which port to establish the connection. This is typically
 *          one of the PORT defines above
 * 
 * Returns 0 if the serial connection was successful
 * Returns SERIAL_ERROR_CONNECTION_FAILED if connection failed
 */
uint8_t serial_init(uint16_t port);

/**
 * serial_write() - Writes a byte of data to the serial port
 * 
 * @port:   Which port to send data to
 * @data:   Byte of data to send
 * 
 * Note this funciton blocks until the pipeline is free
 * Use serial_write_noblock() to avoid this behaviour
 */
void serial_write(uint16_t port, uint8_t data);

/**
 * serial_write() - Writes a string of data to the serial port
 * 
 * @port:   Which port to send data to
 * @data:   String of data to send terminated by \0
 */
void serial_write_string(uint16_t port, char* data);

/**
 * serial_write() - Writes a byte of data without blocking
 * 
 * @port:   Which port to send data to
 * @data:   Byte of data to send
 * 
 * Note that the pipeline should be free prior to calling
 */
void serial_write_noblock(uint16_t port, uint8_t data);

/**
 * serial_is_connected() - Returns whether or not we're connected
 * 
 * Returns 1 if we're connected, 0 if not.
 * This isn't 100% reliable.
 */
uint8_t serial_is_connected(void);


uint8_t serial_is_transmit_empty(uint16_t port);

/**
 * serial_read() - Reads a byte of data off the port.
 * 
 * @port: Port to read
 * 
 * Note that this will return 0 if nothing has been recieved.
 * Since the data we're reading might also be 0, it's recommended
 * to check serial_recieved prior to calling this function
 */
uint8_t serial_read(uint16_t port);

/**
 * serial_recieved() - Indicates whether there's currently data for
 *                      us to read on the port or not
 * 
 * @port:   port to read
 * 
 * Returns 1 if data is availiable, 0 if not.
 */
uint8_t serial_received(uint16_t port);

#endif