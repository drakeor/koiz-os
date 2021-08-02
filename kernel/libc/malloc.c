#include "stdlib.h"
#include "../drivers/memory/pmem.h"

#define SLAB_ENTRY_COUNT 500
#define SLAB_MAX_PAGE_COUNT 20

/* structure for memory slab entries */
struct kmemslab_entry {
    uint32_t mem_addr;
    uint8_t mem_free;
};
typedef struct kmemslab_entry kmemslab_entry_t;


/* structure for a memory slab */
struct kmemslab {
    /* number of free entries */
    uint32_t free_entries;
    /* pointer to page frame that holds entries */
    kmemslab_entry_t entries[SLAB_ENTRY_COUNT];
    /* holds array of all pages currently in use */
    uint32_t pages[SLAB_MAX_PAGE_COUNT];

};
typedef struct kmemslab kmemslab_t;

/* Store slab structures in static memory */
static kmemslab_t slab128;
static kmemslab_t slab256;
static kmemslab_t slab512;
static kmemslab_t slab1024;

void* malloc(uint32_t size)
{
    /* round up to nearest slab */
    /* throw error if more than we can allocate */
    /* if slab has no free entries, then allocate a page (add to pages 
        that are currently allocated and split into entries */
    /* append entries array with new entries */
    /* return first entry */
}

void free(void* ptr)
{
    /* run through all entries and set it to free */
    /* we should also garbage collect at this point... */
    /* this is expensive... oh well, heap is usually slower..
       and we don't expect to free as much in the kernel. */
}