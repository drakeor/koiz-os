#include "ramdisk.h"
#include "../../config.h"
#include "../../libc/stdlib.h"
#include "../../core/mem.h"

uint32_t ramDiskBlockSize = 0;
uint32_t* ramDiskBlockPointers = 0;

/* Size variables (in bytes) */
const uint32_t sector_size = 512;

/* Sectors per cluster */
const uint32_t sectors_per_cluster = 8;

void init_disk_ramdisk(uint32_t sizeInBytes)
{
    /* Error checking */
    if(sizeInBytes == 0)
        panic("init_disk_ramdisk: sizeInBytes cannot be zero!");
    if(sizeInBytes % PHYS_BLOCK_SIZE != 0)
        panic("init_disk_ramdisk: sizeInBytes must be a factor of the blockSize!");
    if(ramDiskBlockSize > 0)
        panic("init_disk_ramdisk: ramDisk already initialized!");
    if(ramDiskBlockPointers > 0)
        panic("init_disk_ramdisk: ramDiskBlockPointers already set!");

    /* Convert to block size */
    ramDiskBlockSize = sizeInBytes / PHYS_BLOCK_SIZE;
    if(ramDiskBlockSize > (PHYS_BLOCK_SIZE / sizeof(uint32_t*)))
        panic("Cannot create that big of a ramdisk!");
    
    /* Allocate blocks of memory for each chunk of the ramdisk */
    ramDiskBlockPointers = pmem_alloc();
    int i;
    for(i = 0; i < ramDiskBlockSize; i++) {
        ramDiskBlockPointers[i] = (uint32_t)pmem_alloc();
    }
}

void destroy_disk_ramdisk()
{
    /* Error checking */
    if(ramDiskBlockSize == 0)
        panic("init_disk_ramdisk: ramDisk is not initialized!");
    if(ramDiskBlockPointers == 0)
        panic("init_disk_ramdisk: ramDiskBlockPointers is already unset!");

    /* Free all the blocks in the list first */
    int i;
    for(i = 0; i < ramDiskBlockSize; i++) {
        pmem_free((uint32_t*)ramDiskBlockPointers[i]);
    }

    /* Free the list itself last */
    pmem_free(ramDiskBlockPointers);

    /* unset variables */
    ramDiskBlockSize = 0;
    ramDiskBlockPointers = 0;
}


void ramdisk_tests()
{
#ifdef SELF_TEST_RAMDISK
    printf("ramdisk: beginning self-tests");

    printf("ramdisk: running test 1...\n");
    init_disk_ramdisk(PHYS_BLOCK_SIZE * 2);
    printf("ramdisk is at %x. block 1: %x. block 2: %x\n", ramDiskBlockPointers, ramDiskBlockPointers[0], ramDiskBlockPointers[1]);
    destroy_disk_ramdisk();

    printf("ramdisk: running test 2...\n");
    init_disk_ramdisk(PHYS_BLOCK_SIZE * 20);
    printf("ramdisk is at %x. block 1: %x. block 20: %x\n", ramDiskBlockPointers, ramDiskBlockPointers[0], ramDiskBlockPointers[19]);
    destroy_disk_ramdisk();

    printf("ramdisk: finished self-tests\n");
#endif
}