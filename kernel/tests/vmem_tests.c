#include <stdint.h>

#include "vmem_tests.h"
#include "../drivers/memory/vmem.h"

#include "../libc/stdlib.h"
#include "../config/config.h"

/* Methods and variables in vmem.c */
extern void* vmem_page_directory;
extern void* vmem_get_phys_addr(uint32_t* proc_page_directory, 
    void* virtual_addr);

void vmem_run_tests()
{
    printf("Running vmem address translations\n");

    /* Look up a test address */
    uint32_t* good_ptr = (uint32_t*) 0x100FFFF;
    printf("Looking up address for %x\n", good_ptr);
    uint32_t* good_ptr_phys = 
        vmem_get_phys_addr(vmem_page_directory, good_ptr);
    printf("Physical address for %x is %x\n", good_ptr, good_ptr_phys);

    /* Make sure it's identity mapped */
    if(good_ptr != good_ptr_phys)
        panic("do_vmem_tests: failed address lookup");

    printf("test passed\n");
    printf("Running vmem access tests\n");

    /* Make sure the memory is accessable. */
    printf("Accessing memory from %x\n", good_ptr);
    printf("Memory value: %x\n", *good_ptr);

    printf("vmem tests finished\n");
}