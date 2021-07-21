#include "../libc/stdlib.h"
#include "../libc/string.h"

#include "../fs/ramdisk_fat16.h"

#include "ramdisk_fat16_tests.h"

void ramdisk_fat16_run_tests()
{
    printf("Running ramdisk.fat16 tests...\n");

    /* Test filenames */
    uint8_t* file_name_1 = (uint8_t*)"rawr.txt";
    uint8_t* file_contents_1 = (uint8_t*)"rawr is the\t name of this file";

    uint8_t* file_name_2 = (uint8_t*)"dragon.txt";
    uint8_t* file_contents_2 = (uint8_t*)"see rawr.txt for more information";

    /* Initialize the ramdisk system */
    ramdisk_fat16_init();

    /* Write a test file to the new file system */
    {
        uint8_t* file_name = file_name_1;
        uint8_t* file_contents = file_contents_1;
        ramdisk_fat16_file_write(file_name, file_contents, 
            strlen(file_contents));
    }
    
    /* Write another test file to the file system */
    {
        uint8_t* file_name = file_name_2;
        uint8_t* file_contents = file_contents_2;
        ramdisk_fat16_file_write(file_name, file_contents, 
            strlen(file_contents));
    }

    /* Check if one of the files exist and show the sector number */
    uint32_t sec_num;
    sec_num = ramdisk_fat16_file_exists(file_name_2);
    if(sec_num == 0)
        panic("Test failed! File 2 doesnt exist and should");
    printf("File 2 exists and is at sector %d\n", sec_num);

    /* TODO: Probably malloc this instead */
    uint8_t sector_buffer[FAT16_BYTES_PER_SECTOR];
    memset(sector_buffer, 0, FAT16_BYTES_PER_SECTOR);

    /* Test reading the first file */
    ramdisk_fat16_file_read(file_name_1, sector_buffer, 0);
    printf("File 1 contents: %s\n", sector_buffer);
    if(strlen(file_contents_1) != strlen(sector_buffer))
        panic("Test failed! File 1 contents lengths do not match!");
    if(memcmp(file_contents_1, sector_buffer, strlen(sector_buffer)) != 0)
        panic("Test failed! File 1 contents are different read than written");

    /* list the ramdisk info */
    ramdisk_fat16_list_info();

    /* clean-up / destroy the ramdisk */
    ramdisk_fat16_destroy();

    printf("Done running ramdisk.fat16 tests...\n");
}