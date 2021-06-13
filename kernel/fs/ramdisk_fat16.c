#include "ramdisk_fat16.h"
#include "../drivers/ramdisk/ramdisk.h"

#define RAMDISK_FAT16_SIZE (4096 * 1024)

int ramdisk_fat16_init()
{
    ramdisk_init(RAMDISK_FAT16_SIZE);
}

int ramdisk_fat16_format()
{

}