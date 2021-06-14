#include "ramdisk_fat16.h"

#include "../drivers/ramdisk/ramdisk.h"
#include "../config/config.h"

#include "../libc/stdlib.h"

/* 
 * A lot of this is hard-coded into ramdisk_fat16.asm
 * So if we want to change anything, we'd have to change it 
 * in ramdisk_fat16.asm. Remember this is only meant to be a testing
 * filesystem but follows the full FAT16 spec.
 * 
 * If we were to re-implement this as a generic file-controller, 
 * we would just redo it all into C structs like normal people do.
 */

#define RAMDISK_FAT16_SIZE (4096 * 1024)

/* Helpful FAT layout map is availiable at:
    https://github.com/rweichler/FAT16/blob/master/spec/FATLayout.pdf */

/* FAT16 Related Constants from ramdisk_fat16.asm */
#define FAT16_RESERVED_SECTOR_COUNT       0x1
#define FAT16_NUMBER_FATS                 0x2
#define FAT16_SECTORS_PER_FAT             0x20
#define FAT16_ROOT_ENTRY_COUNT            0x0200
#define FAT16_TOTAL_SECTORS               0x0200
#define FAT16_SECTORS_PER_CLUSTER         0x1

/* Helpful constants */
#define FIRST_ROOT_SECTOR (FAT16_RESERVED_SECTOR_COUNT + \
                            (FAT16_NUMBER_FATS * FAT16_SECTORS_PER_FAT))

#define ROOT_DIRECTORY_SECTORS ((FAT16_ROOT_ENTRY_COUNT * 32) / 512)

#define FIRST_DATA_SECTOR (FIRST_ROOT_SECTOR + ROOT_DIRECTORY_SECTORS)

#define CLUSTER_COUNT (FAT16_TOTAL_SECTORS - FIRST_DATA_SECTOR) / \
                        FAT16_SECTORS_PER_CLUSTER
/*
 * Grab the hard-coded bootrecord from ramdisk_fat16.asm
 */
extern uint8_t ramdisk_fat16_bootrecord[512];

void ramdisk_fat16_init()
{
    /* Verify that the bootloader is valid */
    if(ramdisk_fat16_bootrecord[510] != 0x55 || 
        ramdisk_fat16_bootrecord[511] != 0xAA) {
            printf("Magic number shown is: %x", 
                (ramdisk_fat16_bootrecord[510] << 8 |
                    ramdisk_fat16_bootrecord[511]));
            panic("Invalid FAT16 magic number in bootloader!");
        }

#ifdef DEBUG_MSG_RAMDISK_FAT16
    printf("ramdisk_fat16_init: First Root Sec: %x | Root Dir Sectors: %x\n",
        FIRST_ROOT_SECTOR, ROOT_DIRECTORY_SECTORS);
    printf("ramdisk_fat16_init: First Data Sec: %x | Cluster Count: %x\n",
        FIRST_DATA_SECTOR, CLUSTER_COUNT);
#endif

    /* Initialize the ramdisk right away */
    ramdisk_init(RAMDISK_FAT16_SIZE);

    /* Show success */

}

void ramdisk_fat16_format()
{

}