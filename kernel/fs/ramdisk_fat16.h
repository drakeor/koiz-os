#ifndef _FAT16_RAMDISK_H_
#define _FAT16_RAMDISK_H_

/* 
 * A lot of this is hard-coded into ramdisk_fat16.asm
 * So if we want to change anything, we'd have to change it 
 * in ramdisk_fat16.asm. Remember this is only meant to be a testing
 * filesystem but should be readable as a FAT16 from other systems.
 * 
 * If we were to re-implement this as a generic file-controller, 
 * we would just redo it all into C structs like normal people do.
 * 
 * Note that all files are stored in the root directory, there is no support
 * for sub-directories with this filesystem
 */

#include <stdint.h>

/* Results enums */
#define FAT16_RAMDISK_SUCCESS                   0
#define FAT16_RAMDISK_ERROR_INVALID_FILENAME    1
#define FAT16_RAMDISK_ERROR_NO_FREE_SPACE       2

/**
 * ramdisk_fat16_init() - Initializes a fat16 system against our ramdisk
 * 
 * Note that this currently this is hardcoded to only read from our ramdisk at
 * the moment. Note this also auto-initializes ramdisk_init()
 */
void ramdisk_fat16_init(void);

/* destroys the current temp ramdisk */
void ramdisk_fat16_destroy(void);

/* lists all files from the ramdisk_fat16 system along with information
    about the filesystem. useful for debugging **/
void ramdisk_fat16_list_info(void);

/* write a file with a certain amount of data. returns zero on success */
int ramdisk_fat16_file_write(uint8_t* file_name, 
    void* data, uint32_t data_size);

/* returns 1 if the file exists and 0 if the file does not exist */
int ramdisk_fat16_file_exists(uint8_t* file_name);

#endif