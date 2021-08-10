#include "stdlib.h"
#include "string.h"
#include "../drivers/memory/pmem.h"

/* Adjust these below as needed if we need more entries/pages in the slabs */
#define SLAB_ENTRY_COUNT 1000
#define SLAB_MAX_PAGE_COUNT 100

/* 
 * Each slab is half of the preceeding slab. 
 * The top-level slab starts at the physical block size 
*/
#define SLAB_COUNT 7

/* structure for memory slab entries */

/*
 * This is the structure for an entry in the memory slab
 * It simply holds an address and a free flag
 */
struct kmemslab_entry {
    uint32_t* mem_addr;
    uint8_t mem_free;
};
typedef struct kmemslab_entry kmemslab_entry_t;

/*
 * This is the main structure for the slab
 * It holds the entries and pages assigned to this slab
 */
struct kmemslab {

    /* Holds the free and used entries in the slab */
    kmemslab_entry_t entries[SLAB_ENTRY_COUNT];
    
    /* Holds the current number of free entries */
    uint32_t free_entries;
    
    /*
     * This is the current entry we're pointing to.
     * This is only used as an optimization
     */
    uint32_t current_entry;

    /*
     * Current soft max number of entries.
     * This will automatically increase up to  SLAB_ENTRY_COUNT
     * as more pages are allocated on the fly
     */
    uint32_t max_entries;

    /* Holds an array of all physical pages assigned to this slab */
    uint32_t* pages[SLAB_MAX_PAGE_COUNT];

    /* Holds the current number of pages assigned to this slab */
    uint32_t current_page;

};
typedef struct kmemslab kmemslab_t;

/* We store all the slab info in static memory */
static kmemslab_t slabs[SLAB_COUNT];

/* 
 * This function lazy-allocates more pages if we need them to fulfill 
 * a malloc request
 */
static void alloc_new_page_if_needed(kmemslab_t* slab, uint32_t slab_size)
{
    /* if we already have free entries, don't need to allocate more */
    if(slab->free_entries > 0)
        return;

    /* Divide up the page into entries */
    int additional_entries = PHYS_BLOCK_SIZE / slab_size;

    /* Prevent overflows, restrict enties if needed */
    int new_max_entries = slab->max_entries + additional_entries;
    if(new_max_entries > SLAB_ENTRY_COUNT) {
        additional_entries = SLAB_ENTRY_COUNT - slab->max_entries;
        new_max_entries = SLAB_ENTRY_COUNT;
    }

    /* Redundant Error checking. Unneeded but.. */
    if((additional_entries * slab_size) > PHYS_BLOCK_SIZE)
        panic("alloc_new_page_if_needed overflow");

    /* If we don't actually have any additional entries to add,
        end here */
    if(additional_entries <= 0)
        return;

    /* If we don't have enough free pages, end here */
    if(slab->current_page == SLAB_MAX_PAGE_COUNT)
        return;
    
    /* Allocate new page */
    uint32_t* c_page = pmem_alloc();
    uint32_t c_addr = (uint32_t)c_page;
    slab->pages[slab->current_page] = c_page;
    ++(slab->current_page);

    /* Split up physical memory block and add records */
    int i;
    for(i = 0; i < additional_entries; i++)
    {
        c_addr += slab_size;
        slab->entries[slab->max_entries].mem_free = 1;
        slab->entries[slab->max_entries].mem_addr = (uint32_t*)c_addr;
        ++(slab->max_entries);
        ++(slab->free_entries);
    }

    /* Last minute paranoia error checking */
    if(slab->max_entries > SLAB_ENTRY_COUNT)
        panic("Max entry overflow!");
    if(c_addr > (c_addr + PHYS_BLOCK_SIZE))
        panic("Malloc allocated more entries than availiable!");
}

void* malloc(uint32_t size)
{
    kmemslab_t* c_slab = NULL;
    int c_size = 0;
    
    /* We want to fit the requested size into the smallest slab we can */
    int current_size = PHYS_BLOCK_SIZE;
    int i;
    for(i = 0; i < SLAB_COUNT; i++)
    {
        if(size <= current_size) {
            c_size = current_size;
            c_slab = &(slabs[i]);
        }
        current_size /= 2;
    }

    /* throw error if the size is more than our biggest slab */
    if(c_slab == NULL)
        panic("Cannot malloc an entry larger than 4096 bytes!");

    /* Lazy-allocate a new page if needed */
    alloc_new_page_if_needed(c_slab, c_size);

    /* If we still don't have free slots, fatal error! */
    if(c_slab->free_entries <= 0)
        panic("malloc failed! no free memory exists!");

    /*
     * We start with the current_entry pointer since that
     * usually points to a free entry. If it's used, we'll
     * iterate until we find one
     */
    uint32_t c_ptr = c_slab->current_entry;
    do {
        /* Found an entry! Mark as used and return address */
        if(c_slab->entries[c_ptr].mem_free) {
            c_slab->entries[c_ptr].mem_free = 0;
            c_slab->current_entry = c_ptr;
            --(c_slab->free_entries);
            return c_slab->entries[c_ptr].mem_addr;
        }
        /* Increment */
        c_ptr = (c_ptr + 1) % c_slab->max_entries;
    } 
    /* We looped all the way around, we don't actually have a few entry */
    while(c_ptr != c_slab->current_entry);

    /* We cannot fulfill the malloc request */
    panic("Cannot malloc! Out of memory!");
    return NULL;
}

/*
 * Returns 1 if something was freed and 0 if nothing was freed
 */
static int free_ptr_slab(kmemslab_t* slab, void* ptr)
{
    int i;
    for(i = (slab->max_entries - 1); i >= 0; i--)
    {
        /* 
         * Free entry and set current pointer to the freed block so
         * it can be used by malloc() right away 
         */
        if(slab->entries[i].mem_addr == ptr) {
            slab->entries[i].mem_free = 1;
            slab->current_entry = i;
            ++(slab->free_entries);
            return 1;
        }
    }
    return 0;
}

void free(void* ptr)
{
    /* TODO: Optimize this */
    
    /* Run thru each slab until we find the address and free it */
    int current_size = PHYS_BLOCK_SIZE;
    int i;
    for(i = 0; i < SLAB_COUNT; i++)
    {
        if(free_ptr_slab(&(slabs[i]), ptr))
            return;

        current_size /= 2;
    }

    panic("failed to free address");
}

/* Helper function to print info for each slab */
static void print_kmeminfo(kmemslab_t* slab, uint32_t slab_size)
{
    printf("%db | Entries Used: %d/%d | Free: %d | Pages Used: %d/%d\n", 
        slab_size, 
        slab->max_entries - slab->free_entries,
        slab->max_entries,
        slab->free_entries,
        slab->current_page,
        SLAB_MAX_PAGE_COUNT);
}

void kmemlist()
{
    printf("=====KMemList=====\n");
    
    /* Helpful to know how much static storage we're using.. */
    printf("Size of Slab Library: %d kb\n", sizeof(slabs) / 1024);

    /* Run thru each slab */
    int current_size = PHYS_BLOCK_SIZE;
    int i;
    for(i = 0; i < SLAB_COUNT; i++)
    {
        print_kmeminfo(&(slabs[i]), current_size);
        current_size /= 2;
    }
}