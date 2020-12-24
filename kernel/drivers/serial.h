#ifndef _SERIAL_H_
#define _SERIAL_H_

#include "stdint.h"

/* 
 * The first two COM ports are almost always the same on x86 systems 
 * We're only going to deal with two of them.
 */
#define PORT_COM1 0x3F8
#define PORT_COM2 0x2F8

uint8_t init_serial(uint16_t port);

// TODO: Add check to make sure port is initialized!
uint8_t write_serial(uint16_t port, uint8_t data);

// TODO: Add non-blocking version
uint8_t read_serial(uint16_t port);

#endif