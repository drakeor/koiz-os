#ifndef _MEM_H_
#define _MEM_H_

#include "stdint.h"

/* Size of physical blocks of memory to give out */
#define PHYS_BLOCK_SIZE 4096

/* Direct mapped kernel page size. PROBABLY UNUSED NOW */
#define IDENTITY_MAP_SIZE (1024 * 1024)

/* NOTE that the memory block returned is NOT guaranteed to be zero'd out! */
uint32_t* pmem_alloc();

int pmem_free(uint32_t* ptr);

void initialize_memory(void);

#endif