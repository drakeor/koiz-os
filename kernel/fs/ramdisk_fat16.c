#include "ramdisk_fat16.h"

#include "../drivers/ramdisk/ramdisk.h"
#include "../config/config.h"

#include "../libc/stdlib.h"
#include "../libc/string.h"

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
} __attribute__((packed));
typedef struct ramdisk_fat16_entry ramdisk_fat16_entry_t;


#define RAMDISK_FAT16_SIZE (4096 * 1024)

/* Helpful FAT layout map is availiable at:
    https://github.com/rweichler/FAT16/blob/master/spec/FATLayout.pdf */

/* FAT16 Related Constants from ramdisk_fat16.asm */
#define FAT16_BOOTLOADER_SIZE 512

#define FAT16_SECTORS_PER_CLUSTER         1
#define FAT16_BYTES_PER_CLUSTER           FAT16_BYTES_PER_SECTOR * \
                                            FAT16_SECTORS_PER_CLUSTER
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
static inline int write_sector(uint32_t start_sec, uint8_t* data)
{
    return ramdisk_write(start_sec * FAT16_BYTES_PER_SECTOR, data,
        FAT16_BYTES_PER_SECTOR);
}

static inline int write_sector_partial(uint32_t start_sec, uint8_t* data, 
    uint32_t len)
{
    return ramdisk_write(start_sec * FAT16_BYTES_PER_SECTOR, data,
        len);
}

/* helper function to convert sectors to addr on the ramdisk */
/* data needs to be size of bytes per sector */
static inline int read_sector(uint32_t start_sec, uint8_t* data)
{
    return ramdisk_read(start_sec * FAT16_BYTES_PER_SECTOR, data,
        FAT16_BYTES_PER_SECTOR);
}

/* helper function to convert disk address to a sector number */
static inline uint16_t addr_to_sect(uint32_t addr)
{
    return (addr / FAT16_BYTES_PER_SECTOR);
}

/* helper function to convert disk address to a sector offset */
static inline uint32_t addr_to_sec_offset(uint32_t addr)
{
    return (addr % FAT16_BYTES_PER_SECTOR);
}

/* helper function to find a free cluster */
/* returns zero if there's no free clusters */
uint16_t find_next_free_cluster()
{
    uint16_t i = 0;
    uint16_t current_cluster = 0;
    for (i = 0; i < CLUSTER_COUNT; i++)
    {
        /* read the current cluster from the ramdisk */
        /* only using the first FAT table */
        ramdisk_read(
            (FIRST_FAT_TABLE_SECTOR * FAT16_BYTES_PER_SECTOR) +
                (i * sizeof(uint16_t)),
            (uint8_t*) &current_cluster,
            sizeof(uint16_t));
        
        /* cluster marked as 0x0000 is free */
        if(current_cluster == 0x0000)
            return i;
    }
    return 0;
}

/* helper function to find the new free root entry */
/* returns zero if there's no free root entries */
/* first entry is skipped */
uint32_t find_next_free_root_entry()
{
    uint32_t i;
    ramdisk_fat16_entry_t cur_root_entry;
    for (i = 1; i < ROOT_DIRECTORY_SECTORS; i++)
    {
        ramdisk_read(
            (FIRST_ROOT_SECTOR * FAT16_BYTES_PER_SECTOR) +
                (i * sizeof(ramdisk_fat16_entry_t)),
            (uint8_t*) &cur_root_entry,
            sizeof(ramdisk_fat16_entry_t));
        
        /* If the initial character is 0, the entry is availiable */
        if(cur_root_entry.entry_name[0] == 0x00)
            return i;
    }
    return 0;
}
/**
 * ramdisk_fat16_format() - Formats the ramdisk to fat16.
 * 
 * Note that this currently is hardcoded to only format the ramdisk at the 
 * moment
 */
void ramdisk_fat16_format()
{
    /* represents a sector with all zero values */
    uint8_t zero_sector[FAT16_BYTES_PER_SECTOR];
    memset(zero_sector, 0x00, FAT16_BYTES_PER_SECTOR);
    int i;

    /* Write the bootloader */
    ramdisk_write(0x00, ramdisk_fat16_bootrecord, FAT16_BOOTLOADER_SIZE);

    /* zero out the FAT tables */
    for(i = 0; i < FAT16_SECTORS_PER_FAT; i++) {
        write_sector(FIRST_FAT_TABLE_SECTOR + i, zero_sector);
        write_sector(SECOND_FAT_TABLE_SECTOR + i, zero_sector);
    }

    /* zero out the root entries */
    for(i = 0; i < ROOT_DIRECTORY_SECTORS; i++) {
        write_sector(FIRST_ROOT_SECTOR + i, zero_sector);
    }

    /* Write the first cluster as 0xFFF8 */
    /* Write the second cluster as 0xFFFF */
    /* Remember this is stored in little-endian though! */
    uint8_t starting_sectors[4] = { 0xF8, 0xFF, 0xFF, 0xFF };
    write_sector(FIRST_FAT_TABLE_SECTOR, starting_sectors);
    write_sector(SECOND_FAT_TABLE_SECTOR, starting_sectors);
}


/**
 * ramdisk_fat16_filename_to_native() - converts a filename to natively
 *                                        stored filename in FAT16
 * 
 * @file_name:      destination native file buffer (needs to be LENGTH 11)
 * @raw_file_name:  raw filename to convert
 * 
 * the raw_file_name can be any length as it will be truncated down to size
 * returns file_name
 */
uint8_t* ramdisk_fat16_filename_to_native(
    uint8_t* native_file_name, uint8_t* raw_file_name) 
{
    /* Set up the buffers to the file name */
    uint8_t file_name_buffer[8];
    uint8_t file_ext_buffer[3];

    /* Fill buffers with blanks by default */
    memset(file_name_buffer, 0x20, 8);
    memset(file_ext_buffer, 0x20, 3);

     /* Format the file name given to FAT16 */
    int proc_file_name = 1;
    int i = 0;
    int j = 0;

    /* Loop through the file name */
    while(raw_file_name[i] != '\0') {

        /* TODO: Check for valid filename */

        /* Handle file name */
        if(proc_file_name) {

            /* Switch to processing file extension */
            if(raw_file_name[i] == '.') {
                proc_file_name = 0;
                j = 0;
                ++i;
                continue;
            }

            /* Add to filename buffer if we have room */
            if(j < 8) {
                file_name_buffer[j] = raw_file_name[i];
                j++;
            }

        } 
        /* Handle file extension */
        else {
            /* Add to file extension buffer if we have room */
            if(j < 3) {
                file_ext_buffer[j] = raw_file_name[i];
                j++;
            }
        }
        ++i;
    }

    memcpy(native_file_name, file_name_buffer, 8);
    memcpy(native_file_name + 8, file_ext_buffer, 3);

    return native_file_name;
}

/* should probably return the root entry num that the file_name exists at... */
/* returns 0 if the file doesnt exist */
/* return the root entry number if the file exists */
uint32_t ramdisk_fat16_file_exists(uint8_t* file_name)
{
    uint32_t i;
    ramdisk_fat16_entry_t cur_root_entry;
    for (i = 1; i < ROOT_DIRECTORY_SECTORS; i++)
    {
        ramdisk_read(
            (FIRST_ROOT_SECTOR * FAT16_BYTES_PER_SECTOR) +
                (i * sizeof(ramdisk_fat16_entry_t)),
            (uint8_t*) &cur_root_entry,
            sizeof(ramdisk_fat16_entry_t));
        
        /* If the initial character is not 0, we can scan the sector */
        if(cur_root_entry.entry_name[0] != 0x00)
        {
            /* Check if the file exists */
            uint8_t native_file_name[11];
            ramdisk_fat16_filename_to_native(native_file_name, file_name);
            if(memcmp(native_file_name, cur_root_entry.entry_name, 11) == 0)
            {
                return i;
            }
        }
    }

    return 0;
}


int ramdisk_fat16_file_read(
    uint8_t* file_name, void* sector_buffer, uint16_t sector_index)
{
    /* check if file exists in root directory */
    uint32_t root_entry_num = ramdisk_fat16_file_exists(file_name);
    if(root_entry_num == 0)
        return FAT16_RAMDISK_ERROR_FILE_DOESNT_EXIST;
    
    /* grab file record */
    ramdisk_fat16_entry_t entry;
    ramdisk_read(
        (FIRST_ROOT_SECTOR * FAT16_BYTES_PER_SECTOR) + 
            (root_entry_num * sizeof(ramdisk_fat16_entry_t)), 
        (uint8_t*) &entry, 
        sizeof(ramdisk_fat16_entry_t));

#ifdef DEBUG_MSG_RAMDISK_FAT16
        /* Note that native_file_name isn't null terminated so we'll
           have garbage at the end */
        printf("ramdisk_fat16_file_read: Reading file %s on cluster %x\n", 
            entry.entry_name, entry.entry_low_cluster_number);
#endif

    /* grab first sector and populate buffer */
    uint16_t current_sector = entry.entry_low_cluster_number;
    read_sector(FIRST_DATA_SECTOR + current_sector, 
            (uint8_t*)sector_buffer);

    /* iterate thru clusters until we find the one the user wants */
    /* TODO: Implement this */
    /* We want to read the FAT table and iterate through clusters until we 
    get the one we want */
    if(sector_index != 0)
        return FAT16_RAMDISK_DOESNT_SUPPORT_MULTICLUSTER;

    /* returns pointer to sector_buffer */
    return FAT16_RAMDISK_SUCCESS;
}


/* writes a file to the fat device */
int ramdisk_fat16_file_write(uint8_t* file_name, void* data, uint32_t data_size)
{


    /* convert the filename to the name and extension used by FAT16 */
    uint8_t native_file_name[11];
    ramdisk_fat16_filename_to_native(native_file_name, file_name);

#ifdef DEBUG_MSG_RAMDISK_FAT16
        /* Note that native_file_name isn't null terminated so we'll
           have garbage at the end */
        printf("ramdisk_fat16_file_write: Writing file %s\n", 
            native_file_name);
#endif

    /* Make sure the file doesn't already exist! */
    if(ramdisk_fat16_file_exists(file_name) != 0)
        return FAT16_RAMDISK_ERROR_FILE_EXISTS;
        
    /* Create a new struct for our file */
    ramdisk_fat16_entry_t new_entry;
    memcpy(new_entry.entry_name, native_file_name, 11);
    new_entry.entry_attrib = 0x00;
    new_entry.entry_reserved = 0x00;
    new_entry.file_size = data_size;

    /* First allocate free sectors */
    uint16_t current_sector = find_next_free_cluster();
    uint16_t head_sector = current_sector;
    uint16_t last_sector = current_sector;

    uint32_t bytes_left_to_write = data_size;
    uint32_t current_data_pointer = 0;
    
    /* Make sure we actually have a free cluster */
    if(current_sector == 0)
        return FAT16_RAMDISK_ERROR_NO_FREE_SPACE;

    /* If our data left to write is bigger than the cluster, 
        fill up the cluster and move to the next one */
    while(bytes_left_to_write > FAT16_BYTES_PER_CLUSTER) {
        
#ifdef DEBUG_MSG_RAMDISK_FAT16
        printf("ramdisk_fat16_file_write: Writing complete data to sector %x\n", 
            current_sector);
#endif

        /* Write entire contents of current sector to data sector */
        write_sector(FIRST_DATA_SECTOR + current_sector, 
            (uint8_t*)data + current_data_pointer);
        
        /* Grab next free sector */
        last_sector = current_sector;
        current_sector = find_next_free_cluster();
        if(current_sector == 0) {
            /* TODO: Add a way to undo our progress */
            printf("ramdisk_fat16_file_write: no way to undo partial write\n");
            panic("ramdisk_fat16_file_write: cannot undo partial write!\n");
        }

        /* Write current sector to FAT */
        uint16_t swapped_csec = eswap_uint16(current_sector);
        ramdisk_write((FIRST_FAT_TABLE_SECTOR * FAT16_BYTES_PER_SECTOR) + 
            (last_sector * 2), (uint8_t*)&swapped_csec, 2);
        ramdisk_write((SECOND_FAT_TABLE_SECTOR * FAT16_BYTES_PER_SECTOR) + 
            (last_sector * 2), (uint8_t*)&swapped_csec, 2);
        
        current_data_pointer += FAT16_BYTES_PER_CLUSTER;
        bytes_left_to_write -= FAT16_BYTES_PER_CLUSTER;
    }

    /* We're now left with only a partial write or nothing to write at all */
    if(bytes_left_to_write > 0) {
#ifdef DEBUG_MSG_RAMDISK_FAT16
        printf("ramdisk_fat16_file_write: Writing %d bytes data to sector %x\n", 
            bytes_left_to_write, current_sector);
#endif
        write_sector_partial(FIRST_DATA_SECTOR + current_sector, 
            ((uint8_t*)data) + current_data_pointer,
            bytes_left_to_write);
    }

    /* Regardless, mark our current sector as the last sector in the chain */
    uint16_t swapped_c_end_value = eswap_uint16(0xFFF8);
    ramdisk_write((FIRST_FAT_TABLE_SECTOR * FAT16_BYTES_PER_SECTOR) + 
            (current_sector * 2), (uint8_t*) &swapped_c_end_value, 2);
    ramdisk_write((SECOND_FAT_TABLE_SECTOR * FAT16_BYTES_PER_SECTOR) + 
            (current_sector * 2), (uint8_t*) &swapped_c_end_value, 2);

    /* Populate the entry with the head to our sector */
    new_entry.entry_low_cluster_number = head_sector;
    
    /* Write the entry to our root directory */
    /* TODO: Probably check if the root directory is already free... 
        earlier before we get to this point... or we'll write the data
        and not actually have room to write the entry! */
    uint16_t new_root_entry = find_next_free_root_entry();
#ifdef DEBUG_MSG_RAMDISK_FAT16
    printf("ramdisk_fat16_file_write: writing %s to root entry number %d\n",
        new_entry.entry_name, new_root_entry);
#endif 
    ramdisk_write(
        (FIRST_ROOT_SECTOR * FAT16_BYTES_PER_SECTOR) + 
            (new_root_entry * sizeof(ramdisk_fat16_entry_t)), 
        (uint8_t*) &new_entry, 
        sizeof(ramdisk_fat16_entry_t));

    /* return success */
    return 0;
}

void ramdisk_fat16_list_info()
{
    /* list information about the filesystem */
    printf("Filesize of FAT16 Ramdisk: %d MB\n", 
        RAMDISK_FAT16_SIZE / 1024 / 1024);

    printf("Total Sectors: %d | Bytes per Cluster: %d\n", 
        FAT16_TOTAL_SECTORS,
        FAT16_BYTES_PER_SECTOR);

    printf("FAT Sectors: %d | Root Sectors: %d | Data Clusters: %d\n", 
        FAT16_RESERVED_SECTOR_COUNT,
        ROOT_DIRECTORY_SECTORS,
        CLUSTER_COUNT);

    /* list free / taken up clusters */
    uint16_t current_cluster = 0;
    uint16_t free_clusters = 0;

    /* Go through the count the number of free clusters */
    int i;
    for(i = 0; i < CLUSTER_COUNT; i++)
    {
        /* read the current cluster from the ramdisk */
        /* only using the first FAT table */
        ramdisk_read(
            (FIRST_FAT_TABLE_SECTOR * FAT16_BYTES_PER_SECTOR) +
                (i * sizeof(uint16_t)),
            (uint8_t*) &current_cluster,
            sizeof(uint16_t));
        
        /* cluster marked as 0x0000 is free */
        if(current_cluster == 0x0000)
            ++free_clusters;
    }

    printf("Used Clusters : %d | Free Clusters %d\n",
        CLUSTER_COUNT - free_clusters, free_clusters);

    /* list all files in the root directory sector */
    printf("\n===File List===\n");
    ramdisk_fat16_entry_t cur_root_entry;
    for(i = 0; i < ROOT_DIRECTORY_SECTORS; i++)
    {
        ramdisk_read(
            (FIRST_ROOT_SECTOR * FAT16_BYTES_PER_SECTOR) +
                (i * sizeof(ramdisk_fat16_entry_t)),
            (uint8_t*) &cur_root_entry,
            sizeof(ramdisk_fat16_entry_t));

        /* We know the sector is free since the first character of the name
           isn't the null character */
        if(cur_root_entry.entry_name[0] != 0x00) {
            uint16_t cluster_number = cur_root_entry.entry_low_cluster_number;
            printf("File %d: %s | Size: %d bytes | Cluster: %d\n", 
                i, 
                cur_root_entry.entry_name,
                cur_root_entry.file_size,
                cluster_number);
        }
    }
    printf("\n");

    /* TODO: list orphaned clusters */
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

void ramdisk_fat16_destroy()
{
    ramdisk_destroy();
}

