#include "../config.h"
#include "mem.h"
#include "stdint.h"
#include "../libc/stdlib.h"

/* This is where our bootloader stores the memory map location */
#define BIOS_MEMORY_MAP_SIZE_LOC 0x8000
#define BIOS_MEMORY_MAP_LOC 0x8004

/* Reserved section of memory */
#define MEM_RESERVED_SECTION_MIN 0x10000

#define MIN_PHYS_BLOCK_SIZE 4096
#define MAX_PHYS_BLOCK_ORDER 4
/*
 * 2^0 = 4KB
 * 2^1 = 8KB
 * 2^2 = 16KB
 * 2^3 = 32KB
 */

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

/*
 * This is where our main physical memory is stored
 * 
 */

uint32_t* main_memory_start;
uint32_t main_memory_length;

/* 
 * Region Types
 * Type 1: Usable (normal) RAM
 * Type 2: Reserved - unusable
 * Type 3: ACPI reclaimable memory
 * Type 4: ACPI NVS memory
 * Type 5: Area containing bad memory
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
        if(map_entry->region_type == 1 && map_entry->region_length_low > main_memory_length)
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

    /* Print our memory info */
    printf("Memory Start: %x, Memory Length: %x\n", main_memory_start, main_memory_length);

    /* 
     * Last few sanity checks 
     * Kernel uses under MEM_RESERVED_SECTION_MIN so that's always reserved
     * We need a section of length 0 as well
     */
    if((uint32_t)main_memory_start < MEM_RESERVED_SECTION_MIN)
        panic("invalid memory start region!");
    if(main_memory_length == 0x00)
        panic("cannot have a memory of length 0!");

}