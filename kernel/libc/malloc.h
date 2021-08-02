#ifndef _MALLOC_H_
#define _MALLOC_H_

#include <stdint.h>

/* allocates dynamic memory on the heap */
/* note that the kernel malloc is super crude and rounds up to the nearest
   byte count. Therefore, all allocations should be as close as possible to
   a power of two. */
void* malloc(uint32_t size);

/* frees memory related to the ptr. will auto-free underlying pages too */ 
void free(void* ptr);

/* lists current memory allocations */
void kmemlist(void);

#endif