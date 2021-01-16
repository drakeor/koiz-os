#ifndef _MEM_H_
#define _MEM_H_

#include "stdint.h"

uint32_t* pmem_alloc();

int pmem_free(uint32_t* ptr);

void initialize_memory(void);

#endif