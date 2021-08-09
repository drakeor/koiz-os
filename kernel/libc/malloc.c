#include "stdlib.h"
#include "string.h"
#include "../drivers/memory/pmem.h"

#define SLAB_ENTRY_COUNT 500
#define SLAB_MAX_PAGE_COUNT 20

#define SLAB_KMEMINFO_PAD_LENGTH 8

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
    /* current page count */
    uint32_t current_page;
    /* current entry count */
    uint32_t current_entry;
};
typedef struct kmemslab kmemslab_t;

/* Store slab structures in static memory */
static kmemslab_t slab128 = {0};
static kmemslab_t slab256 = {0};
static kmemslab_t slab512 = {0};
static kmemslab_t slab1024 = {0};

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

static void print_kmeminfo(kmemslab_t* slab, const char* slab_name)
{
    char slab_name_padded[SLAB_KMEMINFO_PAD_LENGTH];
    memcpy(slab_name_padded, slab_name, SLAB_KMEMINFO_PAD_LENGTH);
    rpad(slab_name_padded, ' ', SLAB_KMEMINFO_PAD_LENGTH);

    printf("%s | Entries Used: %d/%d | Free: %d | Pages Used: %d/%d\n", 
        slab_name_padded, 
        slab->current_entry,
        SLAB_ENTRY_COUNT,
        slab->free_entries,
        slab->current_page,
        SLAB_MAX_PAGE_COUNT);
}

void kmemlist()
{
    printf("=====KMemList=====\n");
    print_kmeminfo(&slab128, "128b");
    print_kmeminfo(&slab128, "256b");
    print_kmeminfo(&slab128, "512b");
    print_kmeminfo(&slab128, "1024b");
}