#ifndef _STRING_H_
#define _STRING_H_

#include <stdint.h>

/* Pretty common macro */
#define NULL 0

/**
 * strlen() - Gets the length of a string
 * 
 * @str:    string to get the length of
 */
uint32_t strlen(const uint8_t* str);

/**
 * memcpy() - Copies memory from one area to another
 * 
 * @dst:    Destination memory address
 * @src:    Source memory address
 * 
 * Returns the pointer to the destination address
 */
void* memcpy(void* dst, void* src, uint32_t data_size);

/**
 * memset() - Sets the first num bytes of the block to a specified value
 * 
 * @ptr:    Pointer to the memory address
 * @value:  Data to set
 * @len:    Length of data to set
 * 
 * Returns @ptr regardless
 */
void* memset(void* ptr, uint8_t value, uint32_t data_size);

/**
 * memcmp() - Compares two strings
 * 
 * @ptr_a:      First memory address to compare
 * @ptr_b:      Second memory address to compare
 * @data_size:  Size of the data to compare
 * 
 * Returns -1 if ptr_a < ptr_b
 * Returns 0 if ptr_a = ptr_b
 * Returns 1 if ptr_a > ptr_b
 */
int32_t memcmp(const void* ptr_a, const void* ptr_b, uint32_t data_size);

#endif