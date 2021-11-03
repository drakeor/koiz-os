#include <stdint.h>

#include "vmem.h"
#include "pmem.h"

#include "../../libc/stdlib.h"
#include "../../config/config.h"

/* Global pointer to the kernel page directory */
void* vmem_page_directory = 0;

/* 
 * Function below is implemented in pagedir.asm. It's Important to make 
 * this fastcall so GCC doesn't accidently store in stack and 
 * break our assembly function.
 */
extern void vmem_enable_page_dir(void* page_dir) 
    __attribute__((fastcall));


/**
 * vmem_ptable_new() - Creates a new zeroed out page table
 * 
 * Returns a pointer to the new page-table
 * Everything in the kernel is identity-mapped
 */
void* vmem_ptable_new()
{
    /* 
     * We need to grab a page frame of physical ram to avoid 4kb 
     * static allocation in the kernel as our stack is relatively small
     * This also has the added benefit of giving us an automatically 
     * aligned page
     */
    void* ptr = pmem_alloc();

    /* Double-check that the physical page size is page-aligned to 4096 
       otherwise paging would be break immediately! */
    if(((uint32_t)ptr & 0xFFF) != 0x0)
        panic("vmem_alloc_pmem: page directory needs to be 4096 aligned!");
    
    /* Physical page size should be at least as big as the 
    array where we'll store stuff */
    if(PHYS_BLOCK_SIZE < sizeof(uint32_t[1024]))
        panic("vmem_alloc_pmem: physical block page size isn't large enough");

    /* Nothing yet exists in the page directory. Zero out memory */
    uint32_t i;
    for(i = 0; i < 1024; i++)
        ((uint32_t*)ptr)[i] = 0x0;

    return ptr;
}

/**
 * vmem_get_phys_addr() - Translates a vmem addr to a pmem addr
 * 
 * @proc_page_directory: Pointer to the page directory to use.
 *                       Does not need to be the kernel one.
 * @virtual_addr:        Virtual address to translate
 * 
 * Returns the corresponding physical address
 */
void* vmem_get_phys_addr(uint32_t* proc_page_directory, 
    void* virtual_addr) 
{
    uint32_t page_dir_entry = ((uint32_t)virtual_addr) >> 22;
    uint32_t page_table_entry = ((uint32_t)virtual_addr) >> 12 & 0x03FF;

    /* Obviously should never happen but you never know... */
    if(page_dir_entry > 1024)
        panic("vmem_get_phys_addr: page_dir_entry will overflow");
    if(page_table_entry > 1024)
        panic("vmem_get_phys_addr: page_table_entry will overflow");
    if(proc_page_directory == 0)
        panic("vmem_get_phys_addr: page_directory was never declared!");

#ifdef DEBUG_MSG_VMEM
        printf("vmem_get_phys_addr: v_addr %x | Dir %d | Table %d\n", 
            virtual_addr, page_dir_entry, page_table_entry);
        printf("vmem_get_phys_addr: page directory location: %x\n", 
            proc_page_directory);
#endif

    /* Check if the entry in the page directory exists */
    if(proc_page_directory[page_dir_entry] == 0) {
#ifdef DEBUG_MSG_VMEM
        printf("vmem_get_phys_addr: page directory entry %d does not exist\n", 
            page_dir_entry);
#endif
        return 0;
    }

#ifdef DEBUG_MSG_VMEM
        printf("vmem_get_phys_addr: page directory entry %d exists \n", 
            page_dir_entry);
#endif

    /* Convert the entry to a proper address to access */
    uint32_t* page_table_addr = 
        (uint32_t*) (proc_page_directory[page_dir_entry] & ~0xFFF);

    /* Check if the page table entry exists. If it does not, return 0 */
    if(page_table_addr[page_table_entry] == 0) {
#ifdef DEBUG_MSG_VMEM
        printf("vmem_get_phys_addr: page table entry %d does not exist\n", 
            page_dir_entry);
#endif
        return 0;
    }

    /* It exists, we'll process it into a physical address */
    uint32_t phys_addr = (page_table_addr[page_table_entry] & ~0xFFF) + 
        ((uint32_t)virtual_addr & 0xFFF);

#ifdef DEBUG_MSG_VMEM
        printf("vmem_get_phys_addr: physical address for %x is %x\n", 
            virtual_addr, phys_addr);
#endif 

    return (void*)phys_addr;
}

/*
 * Page directory structure
 * Bits 0 - 0   : 1 - Page is currently in memory. 
 *                0 - Processor ignores the next 31 bits.
 * Bits 1 - 1   : 1 - Page is currently read/write. 0 - Read-Only
 * Bits 2 - 2   : 1 - User accessable, 0 - Supervisor accessable only
 * Bits 3 - 3   : 1 - Enable write-through caching. 
 *                0 - Write-back is enabled instead.
 * Bits 4 - 4   : 1 - Disable Cache. 0 - Enable cache
 * Bits 5 - 5   : 1 - Page has been accessed (read or write too recently). 
 *                    Bit is managed by the OS.
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
 * Bits 6 - 6   : 1 - Dirty, shows the page was written to. 
 *                    Not updated by the CPU, managed by the OS.
 * Bits 7 - 7   : 0
 * Bits 8 - 8   : 1 - Global, prevents TLB from updating address in cache 
 *                    if CR3 is set. Note, that the page global enable bit 
 *                    in CR4 must be set to enable this feature.
 * Bits 9 - 11  : Can be used by the OS
 * Bits 12 - 31 : Points to 4KiB aligned physical address
 */
void vmem_init(void)
{
    int32_t i;

    /* We'll allocate physical memory to create the kernel page directory */
    uint32_t* base_ptr = vmem_ptable_new();

    /* 
     * Identity page the entire kernel-space. This just makes it easier.
     * However, note that user processes are not paged this way
     */
    for(i = 0; i < 1024; i++) {

        /* Set kernel pagetable to present and read/write in the page directory */
        uint32_t* kernel_pt = vmem_ptable_new();
        //base_ptr[i] = ((uint32_t)kernel_pt) | 3;
        // TODO: THIS ALLOWS USERS TO WRITE INTO KERNEL. CHANGE LATER.
        base_ptr[i] = ((uint32_t)kernel_pt) | 7;

        /* Populate the pagetable with identity-map entries */
        uint32_t j;
        for(j = 0; j < 1024; j++) {
            uint32_t virt_addr_entry = ((i << 22) | (j << 12)) & ~0xFFF;
            //kernel_pt[j] = virt_addr_entry | 1;
            // TODO: THIS ALLOWS USERS TO WRITE INTO KERNEL. CHANGE LATER.
            kernel_pt[j] = virt_addr_entry | 7;
        }

    }

    /* Set the global page directory variable */
    vmem_page_directory = base_ptr;

    /* Enable the page directory */
    vmem_enable_page_dir(base_ptr);

    printf("Paging Enabled for the entire kernel.\n");
}