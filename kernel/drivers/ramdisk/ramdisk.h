#ifndef _RAMDISK_H_
#define _RAMDISK_H_

#include <stdint.h>

/* ramdisk errors */
#define RAMDISK_ERROR_OUT_OF_BOUNDS 1
#define RAMDISK_ERROR_OVERFLOW      2
#define RAMDISK_ERROR_READ          3
#define RAMDISK_ERROR_WRITE         4

/*
 * This is a very simple ramdisk that's mainly just used for testing.
 * There's quite a few restrictions on size.
 */

/** 
 * init_disk_ramdisk() - initializes a ramdisk with a given size
 * 
 * @size_in_bytes:  size in bytes to make the ramdisk.
 *                  note that it pre-allocates everything.
 * 
 * Note that the size needs to be divisible by the page size or this function
 * will fail! Also note that only one ramdisk can be enabled at a time!
 * Also note that the ramdisk max can only be 1024 * page size at the moment too
 * Creating a bigger one will fail.
 */
void ramdisk_init(uint32_t size_in_bytes);

/** 
 * init_disk_ramdisk() - destroys a ramdisk and frees the corresponding memory
 */
void ramdisk_destroy();



/** 
 * ramdisk_write() - writes information to the ramdisk. NOT thread-safe
 * 
 * @start_addr:     starting address of where to write the data
 * @data:           actual data to write
 * @data_size:      data size (in bytes) to write
 * 
 * Returns 0 on success. Fails if the write is unsuccessful.
 * Will auto fail if the write attempts to write past memory bounds
 */
int ramdisk_write(uint32_t start_addr, uint8_t* data, uint32_t data_size);

/** 
 * ramdisk_read() - reads information from the ramdisk.  NOT thread-safe
 * 
 * @start_addr:     starting address of where to read the data
 * @data:           pointer to where the data should be read into
 * @data_size:      data size (in bytes) to write
 * 
 * Returns 0 on success.
 */
int ramdisk_read(uint32_t start_addr, uint8_t* data, uint32_t data_size);

#endif