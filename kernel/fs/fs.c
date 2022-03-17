#include "fs.h"

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