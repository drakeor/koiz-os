#include "sh_read.h"
#include "../../libc/stdlib.h"
#include "../../libc/string.h"
#include "../../fs/fs.h"
#include "../../libc/malloc.h"

int32_t sh_read(int argc, char* argv[])
{
    /* argument check */
    if(argc < 2) {
        printf("read needs 1 argument: filename");
        return -1;
    }

    uint8_t* file_buffer = (uint8_t*)malloc(4096);

    /* make sure the file exists */
    if(!fs_file_exists((uint8_t*)argv[1])) {
        printf("file does not exist\n");
        return -1;
    }

    /* attempt to write the read in the filesystem */
    fs_file_read((uint8_t*)argv[1], file_buffer, 0);
    printf("%s\n", file_buffer);

    free(file_buffer);
    return 0;
}