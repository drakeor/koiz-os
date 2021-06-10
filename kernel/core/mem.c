#include "../config.h"
#include "mem.h"
#include "../libc/stdlib.h"


/* 
 * Good reference for memory maps 
 * https://wiki.osdev.org/Memory_Map_(x86)
 */

/*
 * Physical memory allocation happens on the largest section of detected free memory
 * It's allocated in 4096 byte chunks using simple flat list allocation
 * Probably switch to buddy allocation later...
 */

/* This is where our bootloader stores the memory map location */
#define BIOS_MEMORY_MAP_SIZE_LOC 0x8000
#define BIOS_MEMORY_MAP_LOC 0x8004

/* Reserved section of memory */
#define MEM_RESERVED_SECTION_MIN 0x10000
#define MIN_MEM_SIZE 0xFFFF

/* Minimum memory to run our operating system */
#define MIN_USERSPACE_SIZE (IDENTITY_MAP_SIZE + MIN_MEM_SIZE)

uint32_t* map_regions_count;

/* Represents an entry in the memory map table */
/* We still grab the high even though we're only a 32 bit system */
struct memory_map_entry_t {
  uint32_t base_address_low;
  uint32_t base_address_high;
  uint32_t region_length_low;
  uint32_t region_length_high;
  uint32_t region_type;
  uint32_t acpi_attribs;
} __attribute__((packed));

struct memory_map_entry_t* map_entry;

/* This is where our main memory is stored */
uint32_t* main_memory_start;
uint32_t main_memory_length;

/* Since the memory allocation blocks take up the first few MBs, this is where we'll malloc */
uint32_t* alloc_start;
uint32_t mem_mgr_reserved_size;

/* This points to the next physical spot on the list */
uint32_t curr_alloc_record;

void pmem_tests(void);

/* 
 * Region Types Reference
 * Type 1: Usable (normal) RAM
 * Type 2: Reserved - unusable
 * Type 3: ACPI reclaimable memory
 * Type 4: ACPI NVS memory
 * Type 5: Area containing bad memory
 */

/*
 * The BIOS populates the memory map at 0x8000
 * We scan this list for the largest free block
 * We allocate the start of this block for the memory manager
 * The rest of it is free for allocation
 */
void initialize_memory(void)
{
    /* Grab the memory map region count */
    map_regions_count = (uint32_t*) BIOS_MEMORY_MAP_SIZE_LOC;
    map_entry = (struct memory_map_entry_t*) BIOS_MEMORY_MAP_LOC;

#ifdef DEBUG_MSG_MEMORYMAP
    printf("memory map size: %x\n", *map_regions_count);
#endif

    /* Sanity checks */
    if(*map_regions_count > 0x1F)
        panic("memory map is too large!");
    if(*map_regions_count == 0)
        panic("no valid memory map regions detected!");
    if(PHYS_BLOCK_SIZE & (PHYS_BLOCK_SIZE - 1) != 0)
        panic("physical block size needs to be a power of 2!");


    /*
     * We're grabbing the largest region of memory for memory operations
     */
    main_memory_start = 0x00;
    main_memory_length = 0x00;

#ifdef DEBUG_MSG_MEMORYMAP
    printf("Base Address | Region Length | Type\n");
#endif

    int i;
    for(i=0; i < *map_regions_count; ++i)
    {
        /* Region 1 is free and we want to grab the largest region we can */
        if(map_entry->region_type == 1 
            && map_entry->region_length_low > main_memory_length)
        {
            main_memory_start = (uint32_t*)map_entry->base_address_low;
            main_memory_length = map_entry->region_length_low;
        }
#ifdef DEBUG_MSG_MEMORYMAP
        printf("%x | %x | %d\n", 
            map_entry->base_address_low, 
            map_entry->region_length_low, 
            map_entry->region_type
        );

        /* Do some weird pointer stuff to get the next entry */
        map_entry = (struct memory_map_entry_t*) ( BIOS_MEMORY_MAP_LOC + 0x18 * i );
#endif
    }

    /* Immediately allocate storage for our physical memory manager */
    mem_mgr_reserved_size = (main_memory_length / PHYS_BLOCK_SIZE);

    /* Start of actual allocation is aligned by PHYS_BLOCK_SIZE bytes. Also add a bit of a buffer. */
    uint32_t start_addr_alloc = (uint32_t)main_memory_start;
    start_addr_alloc += mem_mgr_reserved_size + PHYS_BLOCK_SIZE;
    start_addr_alloc &= ~(PHYS_BLOCK_SIZE - 1);
    alloc_start = (uint32_t*)start_addr_alloc;

    /* Print our memory info */
    printf("Memory Start: %x, Memory Length: %x, Allocatable Start: %x, Number of blocks: %d \n", 
        main_memory_start, main_memory_length, alloc_start, mem_mgr_reserved_size);
    printf("Memory: %d MB\n", main_memory_length / 1024 / 1024);

    /* 
     * Last few sanity checks 
     * Kernel uses under MEM_RESERVED_SECTION_MIN so that's always reserved
     * We need a section of length 0 as well
     */
    if((uint32_t)main_memory_start < MEM_RESERVED_SECTION_MIN)
        panic("invalid memory start region!");
    if(main_memory_length == 0x00)
        panic("cannot have a memory of length 0!");
    if(main_memory_length < MIN_MEM_SIZE)
        panic("not enough memory!");

    /*
     * Run pmem tests
     */
#ifdef SELF_TEST_PMEM
    pmem_tests();
#endif

}

/* Helper function to grab record from ptr */
uint32_t pmem_to_entry(uint32_t* ptr)
{
    if(alloc_start > ptr)
        panic("pmem_to_entry: underflow error!");
    if(main_memory_start + main_memory_length < ptr)
        panic("pmem_to_entry: overflow error!");
    uint32_t working_ptr = (uint32_t) ptr;
    working_ptr -= (uint32_t) alloc_start;
    working_ptr /= PHYS_BLOCK_SIZE;
    return working_ptr;
}

/* Helper function to grab ptr from record */
uint32_t* entry_to_pmem(uint32_t entry)
{
    if(entry > mem_mgr_reserved_size)
        panic("entry_to_pmem: entry out of bounds!");
    uint32_t working_ptr = PHYS_BLOCK_SIZE * entry;
    working_ptr += (uint32_t) alloc_start;
    return (uint32_t*) working_ptr;
}

uint32_t* pmem_alloc()
{
    uint32_t record = curr_alloc_record;

    ++curr_alloc_record;
    if(curr_alloc_record >= mem_mgr_reserved_size)
        curr_alloc_record = 0;

    /* If we loop all the way around, we're out of memory */
    while(record != curr_alloc_record)
    {
        uint8_t* record_ptr = (uint8_t*)main_memory_start + record;

        /* Allocate free page if available */
        if(*record_ptr == 0x00)
        {
            *record_ptr = 0x1;
            uint32_t* pmem_addr = entry_to_pmem(record);
#ifdef DEBUG_MSG_PMEM
            printf("pmem: allocating record %d at record addr %x which points to %x\n", record, record_ptr, pmem_addr);
#endif
            return pmem_addr;
        }

        /* Otherwise, incremement our records */
        ++curr_alloc_record;
        if(curr_alloc_record >= mem_mgr_reserved_size)
            curr_alloc_record = 0;
    }
    

    panic("out of memory!");
}

int pmem_free(uint32_t* ptr)
{
    uint32_t record = pmem_to_entry(ptr);
    uint8_t* record_ptr = (uint8_t*)main_memory_start + record;

    /* Memory is already free */
    if(*record_ptr == 0x0) {
        printf("cannot free address %x", ptr);
        panic("attempted to free memory that's already free!");
    }

    /* Mark record as free */
    *record_ptr = 0x0;
#ifdef DEBUG_MSG_PMEM
            printf("pmem: freeing record %d at record addr %x which points to %x\n", record, record_ptr, ptr);
#endif

    return 0;
}

void pmem_test_record(uint32_t entry_record)
{
    uint32_t* entry_test = entry_to_pmem(entry_record);
    uint32_t reverse_test = pmem_to_entry(entry_test);
    printf("pmem test: %d, addr: %x, rev entry: %d...", entry_record, entry_test, reverse_test);
    if(reverse_test != entry_record)
        panic("entry record lookups failed!");
    if(entry_test < alloc_start)
        panic("physical memory underflow error!");
    if(entry_test > main_memory_start + main_memory_length)
        panic("physical memory overflow error!");  
    printf("passed\n");
}

void pmem_tests(void)
{
    /*
     * Test record lookups
     */
    pmem_test_record(0);
    pmem_test_record(5);
    pmem_test_record(16);
    pmem_test_record(mem_mgr_reserved_size);

    /*
     * Test allocations and frees
     */
    printf("pmem: Testing memory allocations...");
    int i = 0;
    for(i = 0; i < mem_mgr_reserved_size * 2; i++)
    {
        uint32_t* mem_addr = pmem_alloc();
        int res = pmem_free(mem_addr);
        if(res != 0) 
            panic("error freeing memory!");
    }
    printf("Passed.\n");
}