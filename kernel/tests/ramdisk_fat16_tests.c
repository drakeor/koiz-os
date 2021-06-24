#include "../libc/stdlib.h"
#include "../fs/ramdisk_fat16.h"
#include "ramdisk_fat16_tests.h"

void ramdisk_fat16_run_tests()
{
    printf("Running ramdisk.fat16 tests...\n");
    ramdisk_fat16_init();
    ramdisk_fat16_destroy();
    printf("Done running ramdisk.fat16 tests...\n");
}