#include "ramdisk_fat16.h"

#include "../drivers/ramdisk/ramdisk.h"
#include "../config/config.h"

#include "../libc/stdlib.h"

/* Structure for a FAT16 entry */
struct ramdisk_fat16_entry {

    /* Name is 8 characters, extension is 3 characters */
    uint8_t entry_name[11];

    /* 
     * File attributes
     * 0x01 - READ_ONLY
     * 0x02 - HIDDEN
     * 0x04 - SYSTEM
     * 0x08 - VOLUME_ID
     * 0x10 - DIRECTORY
     * 0x20 - ARCHIVE
     * 0x0F - Handle LFN
     */
    uint8_t entry_attrib;

    /* Reserved by Windows NT */
    uint8_t entry_reserved;

    /* Creation time in tenths of a second */
    uint8_t entry_creation_time_tenth_secs;

    /*
     * Time that the file was created.
     * 5 bits - Hour
     * 6 bits - Minutes
     * 5 bits - Seconds (Multiply seconds by 2)
     */
    uint16_t entry_creation_time;

    /*
     * Date that the file was created
     * 7 bits - Year
     * 4 bits - Month
     * 5 bits - Day
     */
    uint16_t entry_creation_date;

    /* Last access date. Same format as creation date */
    uint16_t last_accessed_date;

    /* 
     * High 16 bits of the entry's first cluster number.
     * For FAT 16, this is always zero
     */
    uint16_t entry_high_cluster_number;

    /* Last modification time. Same format as creation time */
    uint16_t last_mod_time;

    /* Last modification date. Same format as creation date */
    uint16_t last_mod_date;

    /* 
     * Low 16 bits of the entry's first cluster number
     * Use number to find the first cluster for the entry
     */
    uint16_t entry_low_cluster_number;

    /*
     * Size of the file in bytes
     */
    uint32_t file_size;
};
typedef struct ramdisk_fat16_entry ramdisk_fat16_entry_t;

/* 
 * A lot of this is hard-coded into ramdisk_fat16.asm
 * So if we want to change anything, we'd have to change it 
 * in ramdisk_fat16.asm. Remember this is only meant to be a testing
 * filesystem but follows the full FAT16 spec.
 * 
 * If we were to re-implement this as a generic file-controller, 
 * we would just redo it all into C structs like normal people do.
 */

#define RAMDISK_FAT16_SIZE (4096 * 1024)

/* Helpful FAT layout map is availiable at:
    https://github.com/rweichler/FAT16/blob/master/spec/FATLayout.pdf */

/* FAT16 Related Constants from ramdisk_fat16.asm */
#define FAT16_BOOTLOADER_SIZE 512

#define FAT16_BYTES_PER_SECTOR            512
#define FAT16_SECTORS_PER_CLUSTER         1
#define FAT16_TOTAL_SECTORS               512

#define FAT16_RESERVED_SECTOR_COUNT       1

#define FAT16_ROOT_ENTRY_COUNT            512

#define FAT16_NUMBER_FATS                 2
#define FAT16_SECTORS_PER_FAT             32


/* Helpful constants */
#define FIRST_FAT_TABLE_SECTOR   FAT16_RESERVED_SECTOR_COUNT
#define SECOND_FAT_TABLE_SECTOR  FAT16_RESERVED_SECTOR_COUNT + \
                                    FAT16_SECTORS_PER_FAT

#define FIRST_ROOT_SECTOR (FAT16_RESERVED_SECTOR_COUNT + \
                            (FAT16_NUMBER_FATS * FAT16_SECTORS_PER_FAT))

#define ROOT_DIRECTORY_SECTORS ((FAT16_ROOT_ENTRY_COUNT * 32) / 512)

#define FIRST_DATA_SECTOR (FIRST_ROOT_SECTOR + ROOT_DIRECTORY_SECTORS)

#define CLUSTER_COUNT (FAT16_TOTAL_SECTORS - FIRST_DATA_SECTOR) / \
                        FAT16_SECTORS_PER_CLUSTER

/*
 * Grab the hard-coded bootrecord from ramdisk_fat16.asm
 */
extern uint8_t ramdisk_fat16_bootrecord[FAT16_BOOTLOADER_SIZE];

/* helper function to convert sectors to addr on the ramdisk */
/* data needs to be size of bytes per sector */
int ramdisk_fat16_write_sector(uint32_t start_sec, uint8_t* data)
{
    return ramdisk_write(start_sec * FAT16_BYTES_PER_SECTOR, data,
        FAT16_BYTES_PER_SECTOR);
}

/**
 * ramdisk_fat16_format() - Formats the ramdisk to fat16.
 * 
 * Note that this currently is hardcoded to only format the ramdisk at the 
 * moment
 */
void ramdisk_fat16_format()
{
    uint8_t zero_sector[FAT16_BYTES_PER_SECTOR] = { 0x00 };
    int i;

    /* Write the bootloader */
    ramdisk_write(0x00, ramdisk_fat16_bootrecord, FAT16_BOOTLOADER_SIZE);

    /* zero out the FAT tables */
    for(i = 0; i < FAT16_SECTORS_PER_FAT; i++) {
        ramdisk_fat16_write_sector(FIRST_FAT_TABLE_SECTOR + i, zero_sector);
        ramdisk_fat16_write_sector(SECOND_FAT_TABLE_SECTOR + i, zero_sector);
    }

    /* zero out the root entries */
    for(i = 0; i < ROOT_DIRECTORY_SECTORS; i++) {
        ramdisk_fat16_write_sector(FIRST_ROOT_SECTOR + i, zero_sector);
    }

    /* Write the first cluster as 0xFFF8 */
    /* Write the second cluster as 0xFFFF */
    /* Remember this is stored in little-endian though! */
    uint8_t starting_sectors[4] = { 0xF8, 0xFF, 0xFF, 0xFF };
    ramdisk_fat16_write_sector(FIRST_FAT_TABLE_SECTOR, starting_sectors);
    ramdisk_fat16_write_sector(SECOND_FAT_TABLE_SECTOR, starting_sectors);
}

#define FAT16_RAMDISK_ERROR_INVALID_FILENAME

/* writes a file to the fat device */
int ramdisk_fat16_file_write(uint8_t* file_name, void* data, uint32_t data_size)
{

    /* Set up the buffers to the file name */
    uint8_t file_name_buffer[8] = 
        {0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20};
    uint8_t file_name_ext[3] = {0x20, 0x20, 0x20};

    /* Format the file name given to FAT16 */
    int proc_file_name = 1;
    int i = 0;
    int j = 0;

    /* Loop through the file name */
    while(file_name[i] != '\0') {

        /* Handle file name */
        if(proc_file_name) {

            /* Switch to processing file extension */
            if(file_name[i] == '.') {
                proc_file_name = 0;
                j = 0;
                ++i;
                continue;
            }

            /* Add to filename buffer if we have room */
            if(j < 8) {
                file_name_buffer[j] = file_name[i];
                j++;
            }

        } 
        /* Handle file extension */
        else {
            /* Add to file extension buffer if we have room */
            if(j < 3) {
                file_name_ext[j] = file_name[i];
                j++;
            }
        }
        ++i;
    }

    /* Create a new struct for our file */
    ramdisk_fat16_entry_t new_entry;
    new_entry.entry_attrib = 0x00;
    new_entry.entry_reserved = 0x00;
    new_entry.file_size = data_size;
    
    /* TODO: Scan for free sector */
    /* TODO: Populate the following below with the correct free sector */
    new_entry.entry_low_cluster_number = 0x0002;
    
    /* TODO: Write data to sectors */
    /* TODO: Mark sectors as used */
    /* TODO: Make sure the file doesn't already exist! */


}


void ramdisk_fat16_init()
{
    /* Verify that the bootloader is valid */
    if(ramdisk_fat16_bootrecord[510] != 0x55 || 
        ramdisk_fat16_bootrecord[511] != 0xAA) {
            printf("Magic number shown is: %x", 
                (ramdisk_fat16_bootrecord[510] << 8 |
                    ramdisk_fat16_bootrecord[511]));
            panic("Invalid FAT16 magic number in bootloader!");
        }

#ifdef DEBUG_MSG_RAMDISK_FAT16
    printf("ramdisk_fat16_init: First Root Sec: %x | Root Dir Sectors: %x\n",
        FIRST_ROOT_SECTOR, ROOT_DIRECTORY_SECTORS);
    printf("ramdisk_fat16_init: First Data Sec: %x | Cluster Count: %x\n",
        FIRST_DATA_SECTOR, CLUSTER_COUNT);
#endif

    /* Initialize the ramdisk right away */
    ramdisk_init(RAMDISK_FAT16_SIZE);

    /* Format the ramdisk with a new FAT16 partition */
    ramdisk_fat16_format();

    /* Show the first clusters */
#ifdef DEBUG_MSG_RAMDISK_FAT16
    uint8_t firstCluster[2]; 
    ramdisk_read(FAT16_BOOTLOADER_SIZE, firstCluster, 2);
    printf("ramdisk_fat16_init: First Cluster Values: %x \n",
        firstCluster[1] << 8 | firstCluster[0] );
#endif

}

