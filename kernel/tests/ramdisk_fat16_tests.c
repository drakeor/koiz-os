#include "../libc/stdlib.h"
#include "../libc/string.h"

#include "../fs/ramdisk_fat16.h"

#include "ramdisk_fat16_tests.h"

void ramdisk_fat16_run_tests()
{
    printf("Running ramdisk.fat16 tests...\n");

    /* Initialize the ramdisk system */
    ramdisk_fat16_init();

    /* Write a test file to the new file system */
    {
        uint8_t* file_name = (uint8_t*)"rawr.txt";
        uint8_t* file_contents = (uint8_t*)"rawr is the\n name of this file";
        ramdisk_fat16_file_write(file_name, file_contents, 
            strlen(file_contents));
    }
    
    /* Write another test file to the file system */
    {
        uint8_t* file_name = (uint8_t*)"dragon.txt";
        uint8_t* file_contents = (uint8_t*)"see rawr.txt for more information";
        ramdisk_fat16_file_write(file_name, file_contents, 
            strlen(file_contents));
    }

    /* list the ramdisk info */
    ramdisk_fat16_list_info();

    /* clean-up / destroy the ramdisk */
    ramdisk_fat16_destroy();

    printf("Done running ramdisk.fat16 tests...\n");
}