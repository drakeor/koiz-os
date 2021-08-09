#include "stdlib.h"
#include "string.h"
#include "../drivers/memory/pmem.h"

#define SLAB_ENTRY_COUNT 500
#define SLAB_MAX_PAGE_COUNT 20

#define SLAB_KMEMINFO_PAD_LENGTH 8

/* structure for memory slab entries */
struct kmemslab_entry {
    uint32_t* mem_addr;
    uint8_t mem_free;
};
typedef struct kmemslab_entry kmemslab_entry_t;

/* structure for a memory slab */
struct kmemslab {
    /* pointer to page frame that holds entries */
    kmemslab_entry_t entries[SLAB_ENTRY_COUNT];
    
    /* holds the number of free pages */
    uint32_t free_entries;
    
    /* current entry we're pointing to. this is most likely
       to be a free entry */
    uint32_t current_entry;

    /* current number of max entries (soft max).
         will increase when more pages are allocated */
    uint32_t max_entries;

    /* holds array of all pages currently in use */
    uint32_t* pages[SLAB_MAX_PAGE_COUNT];

    /* current page count */
    uint32_t current_page;

};
typedef struct kmemslab kmemslab_t;

/* Store slab structures in static memory */
static kmemslab_t slab128 = {0};
static kmemslab_t slab256 = {0};
static kmemslab_t slab512 = {0};
static kmemslab_t slab1024 = {0};

static void alloc_new_page_if_needed(kmemslab_t* slab, uint32_t slab_size)
{
    /* if we already have free entries, don't need to allocate more */
    if(slab->free_entries > 0)
        return;

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

    /* round up to nearest slab */
    c_size = 128;
    if(size < c_size) 
        c_slab = &slab128;
    
    c_size = 256;
    if(size < c_size)
        c_slab = &slab256;
    
    c_size = 512;
    if(size < c_size)
        c_slab = &slab512;

    c_size = 1024;
    if(size < c_size)
        c_slab = &slab1024;

    /* throw error if more than we can allocate */
    if(c_slab == NULL)
        panic("Cannot malloc an entry larger than 1024 bytes!");

    /* lazy-allocate a new page if needed */
    alloc_new_page_if_needed(c_slab, c_size);

    /* If we still don't have free slots, fatal error! */
    if(c_slab->free_entries <= 0)
        panic("malloc failed! no free memory exists!");

    /* start with the current pointer */
    /* iterate thru until we find a free entry */
    uint32_t c_ptr = c_slab->current_entry;
    do {
        /* Found an entry! Mark as used and return address */
        if(c_slab->entries[c_ptr].mem_free) {
            c_slab->entries[c_ptr].mem_free = 0;
            --(c_slab->free_entries);
            return c_slab->entries[c_ptr].mem_addr;
        }
        c_ptr = (c_ptr + 1) % c_slab->max_entries;
    } 
    while(c_ptr != c_slab->current_entry);

    /* if we got this far, we don't actually have a free entry */
    panic("Cannot malloc! Out of memory!");
    return NULL;
}

/* returns 1 if something was freed. 0 if nothing was freed */
static int free_ptr_slab(kmemslab_t* slab, void* ptr)
{
    int i;
    for(i = (slab->max_entries - 1); i >= 0; i--)
    {
        if(slab->entries[i].mem_addr == ptr) {
            slab->entries[i].mem_free = 1;
            ++(slab->free_entries);
            return 1;
        }
    }
    return 0;
}

void free(void* ptr)
{
    /* run through all slabs and all entries and set it to free */
    /* Obviously this is STUPIDLY expensive... */
    /* TODO: Optimize this */
    if(free_ptr_slab(&slab128, ptr))
        return;
    if(free_ptr_slab(&slab256, ptr))
        return;
    if(free_ptr_slab(&slab512, ptr))
        return;
    if(free_ptr_slab(&slab1024, ptr))
        return;
    
    panic("failed to free address");
}

static void print_kmeminfo(kmemslab_t* slab, uint8_t* slab_name)
{
    char slab_name_padded[SLAB_KMEMINFO_PAD_LENGTH];
    memcpy(slab_name_padded, slab_name, SLAB_KMEMINFO_PAD_LENGTH);
    rpad((uint8_t*)slab_name_padded, ' ', SLAB_KMEMINFO_PAD_LENGTH);

    printf("%s | Entries Used: %d/%d | Free: %d | Pages Used: %d/%d\n", 
        slab_name_padded, 
        slab->current_entry,
        slab->max_entries,
        slab->max_entries - slab->current_entry,
        slab->current_page,
        SLAB_MAX_PAGE_COUNT);
}

void kmemlist()
{
    printf("=====KMemList=====\n");
    print_kmeminfo(&slab128, (uint8_t*)"128b");
    print_kmeminfo(&slab128, (uint8_t*)"256b");
    print_kmeminfo(&slab128, (uint8_t*)"512b");
    print_kmeminfo(&slab128, (uint8_t*)"1024b");
}