#ifndef _FAT16_RAMDISK_H_
#define _FAT16_RAMDISK_H_

#include <stdint.h>

/**
 * ramdisk_fat16_init() - Initializes a fat16 system against our ramdisk
 * 
 * Note that this currently this is hardcoded to only read from our ramdisk at
 * the moment. Note this also auto-initializes ramdisk_init()
 */
void ramdisk_fat16_init(void);


#endif