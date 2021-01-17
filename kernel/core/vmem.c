#include "mem.h"
#include "vmem.h"

#include "../libc/stdlib.h"

#include <stdint.h>

uint32_t* page_directory[1024];

void vmem_init(void)
{
    /* Grab a page frame of physical ram to avoid 4kb static allocation in kernel */
    /* This also has the benefit of being automatically aligned */
    uint32_t* base_ptr = pmem_alloc();

    /* Physical page size should page-aligned to 4096 */
    if((uint32_t)base_ptr & 0xFFF != 0x0)
        panic("vmem_init: page directory needs to be 4096 aligned!");
    
    /* Physical page size should be at least as big as the array where we'll store stuff */
    if(PHYS_BLOCK_SIZE < sizeof(uint32_t[1024]))
        panic("vmem_init: physical block page size isn't large enough");

    /* Manually assign the addresses to this array */
    uint32_t i;
    for(i = 0; i < 1024; i++)
        page_directory[i] = base_ptr + i;

    /* Set each entry to "not present", read/write, and supervisor */
    for(i = 0; i < 1024; i++)
    {
        *page_directory[i] = 0x00000002;
    }
}