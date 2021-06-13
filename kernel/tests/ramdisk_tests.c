#include <stdint.h>

#include "../libc/stdlib.h"

#include "../drivers/memory/pmem.h"
#include "../drivers/ramdisk/ramdisk.h"

#include "ramdisk_tests.h"

/* Referenced in ramdisk.c */
extern void** ramdisk_block_pointers; 

void ramdisk_run_tests()
{
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
}