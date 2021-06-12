#include "ramdisk.h"
#include "../../config.h"
#include "../../libc/stdlib.h"
#include "../../core/mem.h"

/* Global variables to hold the ramdisk */
uint32_t ramdisk_block_size = 0;
/* Todo: change this to double-pointer to avoid grossness later... */
void** ramdisk_block_pointers = 0; 

/* Size variables (in bytes) */
const uint32_t sector_size = 512;

/* Sectors per cluster */
const uint32_t sectors_per_cluster = 8;

void ramdisk_init(uint32_t size_in_bytes)
{
    /* Error checking */
    if(size_in_bytes == 0)
        panic("init_disk_ramdisk: size_in_bytes cannot be zero!");
    if(size_in_bytes % PHYS_BLOCK_SIZE != 0)
        panic("init_disk_ramdisk: size_in_bytes must be a factor of the PHYS_BLOCK_SIZE!");
    if(ramdisk_block_size > 0)
        panic("init_disk_ramdisk: ramdisk already initialized!");
    if(ramdisk_block_pointers > 0)
        panic("init_disk_ramdisk: ramdisk_block_pointers already set!");
    if(sizeof(void*) != 4)
        panic("init_disk_ramdisk: init_disk_ramdisk can only work on 32-bit pointers");


    /* Currently, we only support cluster sizes being the same size as
       the block sizes */
    if(sector_size * sectors_per_cluster != PHYS_BLOCK_SIZE)
        panic("init_disk_ramdisk: cluster size must be PHYS_BLOCK_SIZE ");

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
    uint32_t old_block_number = UINT32_MAX;

#ifdef DEBUG_MSG_RAMDISK
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
        /* Update current block number. Currently only used for debugging */
        old_block_number = block_number;

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

void ramdisk_tests()
{
#ifdef SELF_TEST_RAMDISK
    /* Variables we need for tests */
    uint32_t i = 0;
    int result = 0;
        
    /* Allocate a tmp page to write test data from */
    uint32_t* tmp_page_write = pmem_alloc();
    for(i = 0; i < PHYS_BLOCK_SIZE / 4; i++)
        tmp_page_write[i] = i;

    /* Allocate a tmp page to read test data to */
    uint32_t* tmp_page_read = pmem_alloc();
    for(i = 0; i < PHYS_BLOCK_SIZE / 4; i++)
        tmp_page_read[i] = 0;

    printf("ramdisk: beginning self-tests\n");

    /* Test 1 only allocates 5 blocks */
    printf("ramdisk: running test 1...\n");
    ramdisk_init(PHYS_BLOCK_SIZE * 5);
    printf("ramdisk is at %x. block 1: %x. block 5: %x\n", 
        ramdisk_block_pointers, 
        ramdisk_block_pointers[0], 
        ramdisk_block_pointers[4]);
    ramdisk_destroy();


    /* Test 2 allocates 20 blocks */
    printf("ramdisk: running test 2...\n");
    ramdisk_init(PHYS_BLOCK_SIZE * 20);
    printf("ramdisk is at %x. block 1: %x. block 20: %x\n", 
        ramdisk_block_pointers, 
        ramdisk_block_pointers[0], 
        ramdisk_block_pointers[19]);
    ramdisk_destroy();

    /* 
     * Test 3 to write to ramdisk spanning across 2 blocks 
     */
    printf("ramdisk: running test 3...\n");

    ramdisk_init(PHYS_BLOCK_SIZE * 2);

    result = ramdisk_write(PHYS_BLOCK_SIZE / 2, (uint8_t*)tmp_page_write, 
        PHYS_BLOCK_SIZE);
    if(result != 0) {
        printf("ramdisk_write threw error code: %d\n", result);
        panic("test failed!");
    }

    ramdisk_destroy();


    /* 
     * Test 4 to read/write to ramdisk spanning across 2 blocks 
     */
    printf("ramdisk: running test 4...\n");

    ramdisk_init(PHYS_BLOCK_SIZE * 2);

    result = ramdisk_write(PHYS_BLOCK_SIZE / 2, (uint8_t*)tmp_page_write, 
        PHYS_BLOCK_SIZE);
    if(result != 0) {
        printf("ramdisk_write threw error code: %d\n", result);
        panic("test failed!");
    }

    result = ramdisk_read(PHYS_BLOCK_SIZE / 2, (uint8_t*)tmp_page_read, 
        PHYS_BLOCK_SIZE);
    if(result != 0) {
        printf("ramdisk_read threw error code: %d\n", result);
        panic("test failed!");
    }

    for(i = 0; i < PHYS_BLOCK_SIZE / 4; i++) {
        if(tmp_page_read[i] != tmp_page_write[i]) {
            printf("%d : %d = %d", i, tmp_page_read[i], tmp_page_write[i]);
            panic("test failed!");
        }
    }
    printf("\n");

    ramdisk_destroy();

    /* free our allocated tmp pages */
    pmem_free(tmp_page_write);
    pmem_free(tmp_page_read);

    printf("ramdisk: finished self-tests\n");
#endif
}