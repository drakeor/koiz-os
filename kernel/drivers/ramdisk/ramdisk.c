#include "ramdisk.h"
#include "../../config/config.h"
#include "../../libc/stdlib.h"
#include "../../drivers/memory/pmem.h"

/* Global variables to hold the ramdisk */
uint32_t ramdisk_block_size = 0;
/* Since the ramdisk can be spread over blocks sparsely, this keeps track
   of where they are */
void** ramdisk_block_pointers = 0; 


void ramdisk_init(uint32_t size_in_bytes)
{
    /* Error checking */
    if(size_in_bytes == 0)
        panic("init_disk_ramdisk: size_in_bytes cannot be zero!");
    if(size_in_bytes % PHYS_BLOCK_SIZE != 0)
        panic("init_disk_ramdisk: size_in_bytes needs to divide block size!");
    if(ramdisk_block_size > 0)
        panic("init_disk_ramdisk: ramdisk already initialized!");
    if(ramdisk_block_pointers > 0)
        panic("init_disk_ramdisk: ramdisk_block_pointers already set!");
    if(sizeof(void*) != 4)
        panic("init_disk_ramdisk: init_disk_ramdisk only works in 32-bit!");

    /* Convert to block size */
    ramdisk_block_size = size_in_bytes / PHYS_BLOCK_SIZE;
    if(ramdisk_block_size > (PHYS_BLOCK_SIZE / sizeof(uint32_t*)))
        panic("Cannot create that big of a ramdisk!");
    
    /* Allocate blocks of memory for each chunk of the ramdisk */
    ramdisk_block_pointers = pmem_alloc();
    int i;
    for(i = 0; i < ramdisk_block_size; i++) {
        *(ramdisk_block_pointers + i) = pmem_alloc();
    }
}

void ramdisk_destroy()
{
    /* Error checking */
    if(ramdisk_block_size == 0)
        panic("init_disk_ramdisk: ramDisk is not initialized!");
    if(ramdisk_block_pointers == 0)
        panic("init_disk_ramdisk: ramdisk_block_pointers is already unset!");

    /* Free all the blocks in the list first */
    int i;
    for(i = 0; i < ramdisk_block_size; i++) {
        pmem_free(*(ramdisk_block_pointers + i));
    }

    /* Free the list itself last */
    pmem_free(ramdisk_block_pointers);

    /* Unset variables */
    ramdisk_block_size = 0;
    ramdisk_block_pointers = 0;
}



/** 
 * ramdisk_internal_io() - performs an IO function to the ramdisk
 * 
 * @io_type:        0 = READ, 1 = WRITE
 * @start_addr:     starting address of where to read the data
 * @data:           pointer to where the data should be read into
 * @data_size:      data size (in bytes) to write
 * 
 * returns 0 if it succeeds
 */
int ramdisk_internal_io(uint32_t io_type, uint32_t start_addr, 
    uint8_t* data, uint32_t data_size)
{
     /* Error checking */
    if(ramdisk_block_size == 0)
        panic("init_disk_ramdisk: ramDisk is not initialized!");
    if(start_addr + data_size > ramdisk_block_size * PHYS_BLOCK_SIZE)
        return RAMDISK_ERROR_OUT_OF_BOUNDS;
    if(start_addr > UINT32_MAX - data_size)
        return RAMDISK_ERROR_OVERFLOW;

    /* Write data to appropiate memory blocks */
    uint32_t i;

#ifdef DEBUG_MSG_RAMDISK
    /* Hold onto the old block number for future debug messages */
    uint32_t old_block_number = UINT32_MAX;

    /* Debug message to say that we're currently writing into the ramdisk */
    if(io_type == 0) {
        printf("Reading from block %d at disk starting location %x...\n", 
            (uint32_t)(start_addr / PHYS_BLOCK_SIZE), 
            start_addr);
    } else {
        printf("Writing into block %d at disk starting location %x...\n", 
            (uint32_t)(start_addr / PHYS_BLOCK_SIZE), 
            start_addr);
    }
#endif

    /* Since our ramdisk is distributed over multiple blocks in
        memory, we need to make sure we write to them correctly */
    for(i = 0; i < data_size; i++)
    {
        uint32_t current_addr = start_addr + i;
        uint32_t offset = current_addr % PHYS_BLOCK_SIZE;
        uint32_t block_number = (uint32_t)(current_addr / PHYS_BLOCK_SIZE);

#ifdef DEBUG_MSG_RAMDISK
        /* Debug message if we need to switch blocks */
        if(old_block_number != block_number) {
            if(io_type == 0) {
                printf("Reading from block %x at block location %x + %x...\n", 
                    block_number,     
                    ramdisk_block_pointers[block_number],
                    offset);
            } else {
                printf("Writing into block %x at block location %x + %x...\n", 
                    block_number,     
                    ramdisk_block_pointers[block_number],
                    offset);
            }
        }
#endif
        uint8_t* current_page_addr = 
                ramdisk_block_pointers[block_number];

        /* Read into data */
        if(io_type == 0) {
            data[i] = *(current_page_addr + offset);
        }
        /* Write into data */
        else {
            *(current_page_addr + offset) = data[i];
        }

#ifdef DEBUG_MSG_RAMDISK
        /* Update current block number. Currently only used for debugging */
        old_block_number = block_number;
#endif

    }

    return 0;
}

int ramdisk_write(uint32_t start_addr, uint8_t* data, uint32_t data_size)
{
    return ramdisk_internal_io(1, start_addr, data, data_size);
}

int ramdisk_read(uint32_t start_addr, uint8_t* data, uint32_t data_size)
{
    return ramdisk_internal_io(0, start_addr, data, data_size);
}
