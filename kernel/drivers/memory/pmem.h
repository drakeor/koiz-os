#ifndef _PMEM_H_
#define _PMEM_H_

#include "stdint.h"
#include "../../libc/multiboot.h"

/* Size of physical blocks of memory to give out */
#define PHYS_BLOCK_SIZE 4096

/* NOTE that the memory block returned is NOT guaranteed to be zero'd out! */
void* pmem_alloc();

int pmem_free(void* ptr);

void pmem_initialize(void);

void pmem_set_mbd(multiboot_info_t* mbd, uint32_t kernel_reserved_end_addr);

#endif