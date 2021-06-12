#include "mem.h"
#include "vmem.h"

#include "../libc/stdlib.h"
#include "../config.h"

#include <stdint.h>

//uint32_t page_directory[1024];
uint32_t* page_directory = 0;

/* Important to make this fastcall so GCC doesn't accidently store in stack and break everything */
extern void vmem_enable_page_dir(uint32_t* page_dir) __attribute__((fastcall));

/* Use this function to ensure a proper chunk of physical memory is returned */
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

/* Creates a new pagetable and automatically zeroes out the memory */
uint32_t* vmem_ptable_new()
{
    /* Grab a page frame of physical ram to avoid 4kb static allocation in kernel */
    /* This also has the benefit of being automatically aligned (which is a requirement for our ptable) 
       so we don't need to zero out the other bits */
    uint32_t* ptr = vmem_alloc_pmem();

    /* Nothing yet exists in the page directory. */
    uint32_t i;
    for(i = 0; i < 1024; i++)
        ptr[i] = 0x0;

    return ptr;
}

uint32_t* vmem_get_phys_addr(uint32_t* proc_page_directory, uint32_t* virtual_addr) 
{
    uint32_t page_dir_entry = ((uint32_t)virtual_addr) >> 22;
    uint32_t page_table_entry = ((uint32_t)virtual_addr) >> 12 & 0x03FF;

    /* Obviously should never happen but you never know... */
    if(page_dir_entry > 1024)
        panic("vmem_get_phys_addr: page_dir_entry is will overflow");
    if(page_table_entry > 1024)
        panic("vmem_get_phys_addr: page_dir_entry is will overflow");
    if(proc_page_directory == 0)
        panic("vmem_get_phys_addr: page_directory was never declared!");

#ifdef DEBUG_MSG_VMEM
        printf("vmem_get_phys_addr: virtual_addr %x has directory entry %d and table entry %d\n", virtual_addr, page_dir_entry, page_table_entry);
#endif

    
#ifdef DEBUG_MSG_VMEM
        printf("vmem_get_phys_addr: page directory location: %x\n", proc_page_directory);
#endif

    /* Check if the entry in the page directory exists */
    if(proc_page_directory[page_dir_entry] == 0) {
#ifdef DEBUG_MSG_VMEM
        printf("vmem_get_phys_addr: page directory entry %d does not exist\n", page_dir_entry);
#endif
        return 0;
    }

#ifdef DEBUG_MSG_VMEM
        printf("vmem_get_phys_addr: page directory entry %d exists \n", page_dir_entry);
#endif

    /* Convert the entry to a proper address to access */
    uint32_t* page_table_addr = (uint32_t*) (proc_page_directory[page_dir_entry] & ~0xFFF);

    /* Check if the page table entry exists. If it does not, return 0 */
    if(page_table_addr[page_table_entry] == 0) {
#ifdef DEBUG_MSG_VMEM
        printf("vmem_get_phys_addr: page table entry %d does not exist\n", page_dir_entry);
#endif
        return 0;
    }

    /* It exists, we'll process it into a physical address */
    uint32_t phys_addr = (page_table_addr[page_table_entry] & ~0xFFF) + ((uint32_t)virtual_addr & 0xFFF);
#ifdef DEBUG_MSG_VMEM
        printf("vmem_get_phys_addr: physical address for %x is %x\n", virtual_addr, phys_addr);
#endif 
    return (uint32_t*)phys_addr;
}


void do_vmem_tests()
{
    printf("Running vmem address translations\n");

    uint32_t* good_ptr = (uint32_t*) IDENTITY_MAP_SIZE - 3092;
    printf("Looking up address for %x\n", good_ptr);
    uint32_t* good_ptr_phys = vmem_get_phys_addr(page_directory, good_ptr);
    printf("Physical address for %x is %x\n", good_ptr, good_ptr_phys);
    if(good_ptr != good_ptr_phys)
        panic("do_vmem_tests: failed address lookup");

    printf("test passed\n");
    printf("Running vmem access tests\n");

    /* Access memory that is identity mapped */
    printf("Accessing memory from %x\n", good_ptr);
    printf("Memory value: %x\n", *good_ptr);

    printf("vmem tests finished\n");
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
 * Bits 9 - 11  : Can be used by the OS (We'll use them for process ID later)
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
    int32_t i;

    /* We can use ptable_new to create the page directory */
    uint32_t* base_ptr = vmem_ptable_new();

    /* 
     * Identity page the entire kernel-space. This just makes it easier.
     * However, note that user processes are not paged this way
     */
    for(i = 0; i < 1024; i++) {

        /* Set kernel pagetable to present and read/write in the page directory */
        uint32_t* kernel_pt = vmem_ptable_new();
        base_ptr[i] = ((uint32_t)kernel_pt) | 3;

        /* Populate the pagetable with identity-map entries */
        uint32_t j;
        for(j = 0; j < 1024; j++) {
            uint32_t virt_addr_entry = ((i << 22) | (j << 12)) & ~0xFFF;
            kernel_pt[j] = virt_addr_entry | 1;
        }

    }

    /* Set the global page directory variable */
    page_directory = base_ptr;

    /* Enable the page directory */
    vmem_enable_page_dir(base_ptr);

    printf("Paging Enabled. First %d KB of kernel identity-mapped.\n", IDENTITY_MAP_SIZE / 1024);

#ifdef SELF_TEST_VMEM
    do_vmem_tests(); 
#endif

}