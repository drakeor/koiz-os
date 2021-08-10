#ifndef _MALLOC_H_
#define _MALLOC_H_

#include <stdint.h>


/**
 * malloc() - Allocates dynamic memory on the heap
 * 
 * @size:   Size in bytes of memory to allocate.
 * 
 * Note that the kernel malloc function is a super crude slab allocator that
 * rounds up to the nearest power of two. Also note that you can only allocate
 * up to a maximum of a physical page size
 */
void* malloc(uint32_t size);

/**
 * free() - Frees dynamic memory based on the pointer
 * 
 * This makes the block of memory availiable for use again. Note that
 * this will not free the underlying page currently however. Also note
 * that the underlying implementation is incredibly slow.
 * 
 * TODO: Add a lookup table or something to optimize this
 */
void free(void* ptr);

/**
 * kmemlist() - Lists the current slabs in use by the dynamic memory allocator
 */
void kmemlist(void);

#endif