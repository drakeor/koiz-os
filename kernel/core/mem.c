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

/* Size of physical blocks of memory to give out */
#define PHYS_BLOCK_SIZE 4096

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
uint32_t* alloc_ptr;

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

    /* Sanity check */
    if(*map_regions_count > 0x1F)
        panic("memory map is too large!");
    
    if(*map_regions_count == 0)
        panic("no valid memory map regions detected!");
    

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
    mem_mgr_reserved_size = 0xF + (main_memory_length / PHYS_BLOCK_SIZE);
    alloc_ptr = main_memory_start;
    alloc_start = main_memory_start;
    alloc_start += mem_mgr_reserved_size;

    /* Print our memory info */
    printf("Memory Start: %x, Memory Length: %x, Allocatable Start: %x\n", 
        main_memory_start, main_memory_length, alloc_start);

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

}

/* Helper function to grab record from ptr */
uint32_t* pmem_to_entry(uint32_t* ptr)
{

}

/* Helper function to grab ptr from record */
uint32_t* entry_to_pmem(uint32_t* ptr)
{

}

uint32_t* pmem_alloc()
{

}

int pmem_free(uint32_t* ptr)
{

}