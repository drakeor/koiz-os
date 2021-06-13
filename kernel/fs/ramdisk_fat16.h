#ifndef _FAT16_RAMDISK_H_
#define _FAT16_RAMDISK_H_

#include <stdint.h>

/**
 * ramdisk_fat16_init() - Initializes a fat16 system against our ramdisk
 * 
 * Note that this currently this is hardcoded to only read from our ramdisk at
 * the moment. Note this also auto-initializes ramdisk_init()
 */
int ramdisk_fat16_init();

/**
 * ramdisk_fat16_format() - Formats the ramdisk to fat16.
 * 
 * Note that this currently is hardcoded to only format the ramdisk at the 
 * moment
 */
int ramdisk_fat16_format();

#endif