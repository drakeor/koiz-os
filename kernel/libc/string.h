#ifndef _STRING_H_
#define _STRING_H_

#include <stdint.h>

/* Pretty common macro */
#define NULL 0

/**
 * strcpy() - Copies string from src to dst safely, terminating
 *            on \0. Will only partially copy if it starts to overflow
 *            destination
 * 
 * @dst:        Destination string
 * @src:        Source string
 * @dst_size:   Max size of the destination string.    
 */
uint8_t* strcpy(uint8_t* dst, uint8_t* src, uint32_t dst_size);


/**
 * strlen() - Gets the length of a string
 * 
 * @str:    string to get the length of
 */
uint32_t strlen(const uint8_t* str);

/**
 * strlen() - Compares two strings
 * 
 * @str_a: String a to compare
 * @str_b: String b to compare
 * 
 * Behaves just like the standard strcmp function in C library
 */
int32_t strcmp(const char* str_a, const char* str_b);

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

/**
 * rpad() - Pads out a character to length with pad_char
 * 
 * @str:        Pointer to string buffer
 * @pad_char:   Character to pad to
 * @length:     Length to pad out too
 * 
 * Please ensure that @str is at LEAST as large as length!
 * Note that the final character will be set as \0
 */
void rpad(uint8_t* str, char pad_char, int length);

#endif