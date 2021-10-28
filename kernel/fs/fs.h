#ifndef _FS_H_
#define _FS_H_

#include <stdint.h>

/* 
 * This file acts like an abstraction layer between the actual filesystem
 * implementation and the kernel 
 */

/* Reference: 
    https://www.kernel.org/doc/html/latest/filesystems/vfs.html#introduction 
*/

/**
 * fs_list_files() - Lists all files
 * 
 * Lists all files in the current working directory.
 * Since we only support the root directory at the minute,
 * it doesn't matter
 * 
 * TODO: Needs to return generic message vs ramdisk_fat16 specific
 */
int fs_list_files(void);

/**
 * fs_file_write() - Writes a file to the filesystem
 * 
 * @file_name:      name of file to write to
 * @data:           data to write to the file
 * @data_size:      size of the data to write to the file
 * 
 * TODO: Needs to return generic message vs ramdisk_fat16 specific
 */
int fs_file_write(uint8_t* file_name,  void* data, uint32_t data_size);

/**
 * fs_file_exists() - Lists all files
 * 
 * @file_name:      name of file to check
 * 
 * Returns sector number if file exists and 0 if the file does not exist
 * TODO: Needs to return generic message vs ramdisk_fat16 specific
 */
uint32_t fs_file_exists(uint8_t* file_name);

/**
 * fs_file_read() - Reads a file from the filesystem
 * 
 * @file_name:          name of file to write to
 * @sector_buffer:      pointer to buffer to read the file into
 * @sector_index:       which sector index to read from the file
 * 
 * TODO: Needs to return generic message vs ramdisk_fat16 specific
 */
int fs_file_read(
    uint8_t* file_name, void* sector_buffer, uint16_t sector_index);

#endif