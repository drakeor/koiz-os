#include "../../config/config.h"
#include "pmem.h"
#include "../../libc/stdlib.h"


/* 
 * Good reference for memory maps 
 * https://wiki.osdev.org/Memory_Map_(x86)
 */

/*
 * Physical memory allocation happens on the largest section of 
 * detected free memory. It's allocated in 4096 byte chunks using 
 * simple flat list allocation. I should probably switch to 
 * buddy allocation or something later...
 */


/* Reserved section of memory */
#define MEM_RESERVED_SECTION_MIN 0x100000
#define MIN_MEM_SIZE 0xFFFF

/* Flag to determine whether or not pmem is initialized yet */
static int pmem_initialized = 0;

/* This is where our main memory is stored */
static void* pmem_main_memory_start;
static uint32_t pmem_main_memory_length;

/*
 * Since the memory allocation blocks take up the first few MBs, 
 * this is where we'll malloc from
 */
static void* pmem_alloc_start;

/* Number of entries that will be availiable in pmem. This is 
   also used in the test script, hence why it's global */
uint32_t pmem_mgr_reserved_size;

/* This points to the next physical spot on the list */
static uint32_t pmem_curr_alloc_record;

/* Global variables to hold the MBD information */
/* This should never get clobbered */
static multiboot_info_t* pmem_mbd = 0;
static uint32_t pmem_kernel_reserved_end_addr = 0;

void pmem_set_mbd(multiboot_info_t* mbd, uint32_t kernel_reserved_end_addr)
{
    pmem_mbd = mbd;
    pmem_kernel_reserved_end_addr = kernel_reserved_end_addr;
}

int pmem_isinit(void)
{
    return pmem_initialized;
}

void pmem_initialize(void)
{
    /* Sanity checks */
    if(pmem_mbd == 0)
        panic("pmem: MBD was never set!");
    if(pmem_kernel_reserved_end_addr == 0)
        panic("pmem: kernel reserved end addr is 0!");
    if((PHYS_BLOCK_SIZE & (PHYS_BLOCK_SIZE - 1)) != 0)
        panic("physical block size needs to be a power of 2!");
        
    /* Check bit 6 to see if we have a valid memory map */
    if(!(pmem_mbd->flags >> 6 & 0x1)) {
        printf("mbd flags: %b", pmem_mbd->flags);
        panic("invalid memory map given by GRUB bootloader");
    }

    /*
     * We're grabbing the largest region of memory for memory operations
     */
    pmem_main_memory_start = 0x00;
    pmem_main_memory_length = 0x00;

#ifdef DEBUG_MSG_MEMORYMAP
    printf("memory map addr is %x and the length is %d\n",
        pmem_mbd->mmap_addr, pmem_mbd->mmap_length);
#endif

    int i;
    for(i = 0; i < pmem_mbd->mmap_length; 
        i += sizeof(multiboot_memory_map_t)) 
    {
        multiboot_memory_map_t* cs = 
            (multiboot_memory_map_t*) (pmem_mbd->mmap_addr + i);

#ifdef DEBUG_MSG_MEMORYMAP
        printf("Start Addr: %x | Length: %x | Size: %x | Type: %d\n",
            cs->addr_low, cs->len_low, cs->size, cs->type);
#endif

        /* Grab the largest region */
        if(cs->type == MULTIBOOT_MEMORY_AVAILABLE &&
            cs->len_low > pmem_main_memory_length) {
            pmem_main_memory_start = (void*)cs->addr_low;
            pmem_main_memory_length = cs->len_low;
        }
    }

#ifdef DEBUG_MSG_MEMORYMAP
    printf("Main memory start addr: %x | length: %x\n",
            pmem_main_memory_start, pmem_main_memory_length);
#endif

    /* 
     * Okay, now offset pmem_main_memory_start if it's less than 
     * our reserved kernel address space
     */
    if((uint32_t)pmem_main_memory_start < pmem_kernel_reserved_end_addr)
    {
        uint32_t space_to_alloc = 
            (pmem_kernel_reserved_end_addr - (uint32_t)pmem_main_memory_start);

        /* Make sure we have enough space */
        if(space_to_alloc > pmem_main_memory_length)
            panic("Not enough free space outside kernel!");
        
        /* Offset so it's out of our reserved kernel space */
        pmem_main_memory_start = (uint32_t*)pmem_kernel_reserved_end_addr;
        pmem_main_memory_length -= space_to_alloc;
#ifdef DEBUG_MSG_MEMORYMAP
        printf("New main memory start addr: %x | length: %x\n",
            pmem_main_memory_start, pmem_main_memory_length);
#endif
    }

    /* Immediately allocate storage for our physical memory manager */
    uint32_t end_addr = (uint32_t)pmem_main_memory_start 
        + pmem_main_memory_length;
    pmem_mgr_reserved_size = (pmem_main_memory_length / PHYS_BLOCK_SIZE);

    /* Zero out the memory */
    for(i = 0; i < pmem_mgr_reserved_size; i++) {
        uint8_t* record_ptr = (uint8_t*)(pmem_main_memory_start) + i;
        (*record_ptr) = 0x00;
    }

    /* 
     * Add the starting address right after the storage reserved for
     * the memory manager 
     */
    uint32_t start_addr_alloc = (uint32_t)pmem_main_memory_start;
    start_addr_alloc += pmem_mgr_reserved_size + PHYS_BLOCK_SIZE;

    /* Start of actual allocation is aligned by PHYS_BLOCK_SIZE bytes. */
    start_addr_alloc &= ~(PHYS_BLOCK_SIZE - 1);
    pmem_alloc_start = (void*)start_addr_alloc;

    /* Subtract from our length and redo the mgr_reserved_size */
    pmem_main_memory_length = (end_addr - start_addr_alloc);
    pmem_mgr_reserved_size = (pmem_main_memory_length / PHYS_BLOCK_SIZE);


    /* Print our memory info */
    printf("Alloc Start: %x | Length: %x | # of Blocks: %d \n",
        pmem_alloc_start, pmem_main_memory_length, 
        pmem_mgr_reserved_size);
    printf("Free Memory: %d MB\n", pmem_main_memory_length / 1024 / 1024);

    /* 
     * Last few sanity checks 
     * Kernel uses under MEM_RESERVED_SECTION_MIN so that's always reserved
     * We need a section of length 0 as well
     */
    if((uint32_t)pmem_main_memory_start < MEM_RESERVED_SECTION_MIN)
        panic("invalid memory start region!");
    if(pmem_main_memory_length == 0x00)
        panic("cannot have a memory of length 0!");
    if(pmem_main_memory_length < MIN_MEM_SIZE)
        panic("not enough memory!");

    /* We're now initialized */
    pmem_initialized = 1;
}

/**
 * pmem_ptr_to_entry() - Converts a pointer to pmem entry
 * 
 * @ptr: Pointer to physical memory
 * 
 * We don't make this static as tests use it.
 */
uint32_t pmem_ptr_to_entry(void* ptr)
{
    if((uint32_t)pmem_alloc_start > (uint32_t)ptr)
        panic("pmem_ptr_to_entry: underflow error!");
    if((uint32_t)pmem_alloc_start + pmem_main_memory_length <
        (uint32_t)ptr) {
            panic("pmem_ptr_to_entry: overflow error!");
        }

    uint32_t working_ptr = (uint32_t) ptr;
    working_ptr -= (uint32_t) pmem_alloc_start;
    working_ptr /= PHYS_BLOCK_SIZE;

    return working_ptr;
}

/**
 * pmem_ptr_to_entry() - Converts a pmem entry to a pointer
 * 
 * @entry: Entry number to convert to a pointer
 * 
 * We don't make this static as tests use it
 */
void* pmem_entry_to_ptr(uint32_t entry)
{
    if(entry > pmem_mgr_reserved_size)
        panic("pmem_entry_to_ptr: entry out of bounds!");

    uint32_t working_ptr = PHYS_BLOCK_SIZE * entry;
    working_ptr += (uint32_t) pmem_alloc_start;

    return (void*) working_ptr;
}

void* pmem_alloc()
{
    /* Error checking */
    if(!pmem_initialized)
        panic("Call to pmem_alloc before initialized!");
        
    /* Keep track of the start record so we know if we looped
       around the list */
    uint32_t start_record = pmem_curr_alloc_record;

    ++pmem_curr_alloc_record;
    if(pmem_curr_alloc_record >= pmem_mgr_reserved_size)
        pmem_curr_alloc_record = 0;

    /* If we loop all the way around, we're out of memory */
    while(start_record != pmem_curr_alloc_record)
    {
        uint8_t* record_ptr = ((uint8_t*)pmem_main_memory_start) + 
            pmem_curr_alloc_record;

        /* Allocate free page if available */
        if(*record_ptr == 0x00)
        {
            *record_ptr = 0x1;
            void* pmem_addr = pmem_entry_to_ptr(pmem_curr_alloc_record);
#ifdef DEBUG_MSG_PMEM
            printf("pmem: allocating record %d at addr %x for %x\n", 
                start_record, record_ptr, pmem_addr);
#endif
            return pmem_addr;
        }

        /* Otherwise, incremement our records */
        ++pmem_curr_alloc_record;
        if(pmem_curr_alloc_record >= pmem_mgr_reserved_size)
            pmem_curr_alloc_record = 0;
    }
    
    panic("out of memory!");
    return 0;
}

int pmem_free(void* ptr)
{
    uint32_t record = pmem_ptr_to_entry(ptr);
    uint8_t* record_ptr = (uint8_t*)pmem_main_memory_start + record;

    /* Memory is already free */
    if(*record_ptr == 0x0) {
        printf("cannot free address %x", ptr);
        panic("attempted to free memory that's already free!");
    }

    /* Mark record as free */
    *record_ptr = 0x0;
#ifdef DEBUG_MSG_PMEM
    printf("pmem: freeing record %d at  addr %x for %x\n", 
        record, record_ptr, ptr);
#endif

    return 0;
}

void pmem_list_info(void)
{
    printf("Memory Start: %x \n", pmem_main_memory_start);
    printf("Memory Size: %d mb\n", pmem_main_memory_length / 1024 / 1024);

    /* Count up the number of pages in use */
    uint32_t pages_in_use = 0;
    int i;
    for(i = 0; i < pmem_mgr_reserved_size; i++)
    {
        uint8_t* record_ptr = ((uint8_t*)pmem_main_memory_start) + i;
        if(*record_ptr != 0x00)
            ++pages_in_use;
    }
    printf("Pages in Use: %d / %d\n", pages_in_use, pmem_mgr_reserved_size);

    /* Calculate memory in use */
    printf("Memory In Use: %d mb / %d mb\n", 
        (pages_in_use * PHYS_BLOCK_SIZE) / 1024 / 1024,
        pmem_main_memory_length / 1024 / 1024);
}