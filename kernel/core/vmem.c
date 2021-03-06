#include "mem.h"
#include "vmem.h"

#include "../libc/stdlib.h"

#include <stdint.h>

/* uint32_t* page_directory[1024]; */

/* Important to make this fastcall so GCC doesn't accidently store in stack and break everything */
extern void vmem_enable_page_dir(uint32_t* page_dir) __attribute__((fastcall));

/* Use this function to ensure a proper chunk is returned */
uint32_t* vmem_alloc_pmem()
{
    /* Grab a page frame of physical ram to avoid 4kb static allocation in kernel */
    /* This also has the benefit of being automatically aligned (or should be) */
    uint32_t* ptr = pmem_alloc();

    /* Double-check that the physical page size should be page-aligned to 4096 otherwise paging would be weird! */
    if((uint32_t)ptr & 0xFFF != 0x0)
        panic("vmem_init: page directory needs to be 4096 aligned!");
    
    /* Physical page size should be at least as big as the array where we'll store stuff */
    if(PHYS_BLOCK_SIZE < sizeof(uint32_t[1024]))
        panic("vmem_init: physical block page size isn't large enough");

    return ptr;
}

uint32_t* vmem_ptable_new()
{
    /* Grab a page frame of physical ram to avoid 4kb static allocation in kernel */
    /* This also has the benefit of being automatically aligned */
    uint32_t* ptr = vmem_alloc_pmem();

    /* Nothing yet exists in the page directory. */
    uint32_t i;
    for(i = 0; i < 1024; i++)
        ptr[i] = 0x0;

    return ptr;
}

/*
 * Page directory structure
 * Bits 0 - 0   : 1 - Page is currently in memory. 0 - Processor ignores the next 31 bits.
 * Bits 1 - 1   : 1 - Page is currently read/write. 0 - Read-Only
 * Bits 2 - 2   : 1 - User accessable, 0 - Supervisor accessable only
 * Bits 3 - 3   : 1 - Enable write-through caching. 0 - Write-back is enabled instead.
 * Bits 4 - 4   : 1 - Disable Cache. 0 - Enable cache
 * Bits 5 - 5   : 1 - Page has been accessed (read or write too recently). Bit is managed by the OS.
 * Bits 6 - 6   : 0
 * Bits 7 - 7   : 0 - 4kb page size (what we use)
 * Bits 8 - 8   : Ignored.
 * Bits 9 - 11  : Can be used by the OS
 * Bits 12 - 31 : 4-kb aligned address of page table
 */

/*
 * Page table structure
 * Bits 0 - 0   : Present like above
 * Bits 1 - 1   : Read/Write like above
 * Bits 2 - 2   : User/Supervisor like above
 * Bits 3 - 3   : Write-through like above
 * Bits 4 - 4   : Cache Disabled like above
 * Bits 5 - 5   : Accessed like above
 * Bits 6 - 6   : 1 - Dirty, shows the page was written to. Not updated by the CPU, managed by the OS.
 * Bits 7 - 7   : 0
 * Bits 8 - 8   : 1 - Global, prevents TLB from updating address in cache if CR3 is set.
 *                    Note, that the page global enable bit in CR4 must be set to enable this feature.
 * Bits 9 - 11  : Can be used by the OS
 * Bits 12 - 31 : Points to 4KiB aligned physical address
 */
void vmem_init(void)
{
    /* We can use ptable_new to create the page directory */
    uint32_t* base_ptr = vmem_ptable_new();

    /* Identity page the first 4 megabytes */
    uint32_t* kernel_pt = vmem_ptable_new();
    int32_t i;
    for(i = 0; i < 1024; i++)
        kernel_pt[i] = (i * 4096) | 1;

    base_ptr[0] = ((uint32_t)kernel_pt) | 1;

    /* Enable the page directory */
    vmem_enable_page_dir(base_ptr);

    printf("Paging Enabled. First 4MB of kernel identity-mapped.\n");

    /*uint32_t* bad_ptr = (uint32_t*)0x01FF0000;
    printf("Accessing bad memory from %x\n", *bad_ptr);*/
}