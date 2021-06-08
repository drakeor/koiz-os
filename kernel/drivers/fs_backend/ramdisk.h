#ifndef _RAMDISK_H_
#define _RAMDISK_H_

#include <stdint.h>

void init_disk_ramdisk(uint32_t sizeInBytes);

void destroy_disk_ramdisk();

void ramdisk_tests();

#endif