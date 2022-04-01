#include "pmem_tests.h"
#include <stdint.h>
#include "../libc/stdlib.h"
#include "../drivers/memory/pmem.h"

uint32_t pmem_ptr_to_entry(void* ptr);
void* pmem_entry_to_ptr(uint32_t entry);

extern uint32_t pmem_mgr_reserved_size;

/**
 * pmem_test_record() - Helper function to test each record
 * 
 * @entry_record:   Record to test
 */
void pmem_test_record(uint32_t entry_record)
{
    uint32_t* entry_test = pmem_entry_to_ptr(entry_record);
    uint32_t reverse_test = pmem_ptr_to_entry(entry_test);

    printf("pmem test: %d, addr: %x, rev entry: %d...", 
        entry_record, entry_test, reverse_test);

    if(reverse_test != entry_record)
        panic("entry record lookups failed!");

    printf("passed\n");
}

void pmem_run_tests(void)
{
    /*
     * Test record lookups
     */
    printf("pmem: Testing record lookups...\n");
    pmem_test_record(0);
    pmem_test_record(5);
    pmem_test_record(16);
    printf("pmem: Testing Record lookups Passed.\n");
    
    /*
     * Test allocations and frees
     */
     
#ifdef DEBUG_SELF_TEST_PMEM_ALLOCATIONS
    printf("pmem: Testing memory allocations...\n");
    int i = 0;
    for(i = 0; i < pmem_mgr_reserved_size * 2; i++)
    {
        /* Allocate and immediately free the memory */
        uint32_t* mem_addr = pmem_alloc();
        int res = pmem_free(mem_addr);
        if(res != 0) 
            panic("error freeing memory!");
        
    }
    printf("pmem: Testing Memory Allocations Passed.\n");
#endif 

}