#include "fs.h"
#include "../libc/stdlib.h"
#include "../libc/string.h"
#include "ramdisk_fat16.h"

int fs_list_files()
{
    ramdisk_fat16_list_info();
    return 0;
}

int fs_file_write(uint8_t* file_name,  void* data, uint32_t data_size)
{
    return ramdisk_fat16_file_write(file_name, data, data_size);
}

uint32_t fs_file_exists(uint8_t* file_name)
{
    return ramdisk_fat16_file_exists(file_name);
}

int fs_file_read(
    uint8_t* file_name, void* sector_buffer, uint16_t sector_index)
    {
        return ramdisk_fat16_file_read(file_name, sector_buffer, sector_index);
    }

int fs_load_starting_files()
{
    /* Write the test program to the new file system */
    {
        uint8_t* file_name = 
            (uint8_t*)"test.bin";
        uint8_t file_contents[] =
            {
                /* Code */
                0x66, 0xbf, 0x1c, 
                0x00, 0x00, 0x00, 

                0x66, 0xb8, 0x01, 
                0x00, 0x00, 0x00, 

                0xcd, 0x33, 

                0x66, 0xbf, 0x00, 
                0x00, 0x00, 0x00, 

                0x66, 0xb8, 0x02, 
                0x00, 0x00, 0x00, 

                0xcd, 0x33, 

                /* Data */
                0x48, 0x65, 0x6c, 0x6c,
                0x6f, 0x20, 0x66, 0x72, 
                0x6f, 0x6d, 0x20, 0x74, 
                0x65, 0x73, 0x74, 0x20, 
                0x70, 0x72, 0x6f, 0x67,
                0x72, 0x61, 0x6d, 0x21, 
                0x0a, 0x00
            };

        int res = ramdisk_fat16_file_write(file_name, file_contents, 
            sizeof(file_contents));

        if(res != 0)
            panic("Failed to write starting file test.bin!");
    }

    printf("Loaded starting files to disk!\n");
    return 0;
}