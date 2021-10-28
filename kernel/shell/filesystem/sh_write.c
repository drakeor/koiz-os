#include "sh_write.h"
#include "../../libc/stdlib.h"
#include "../../libc/string.h"
#include "../../fs/fs.h"

int32_t sh_write(int argc, char* argv[])
{
    /* argument check */
    if(argc < 3) {
        printf("write needs 2 arguments: filename and contents");
        return -1;
    }

    /* attempt to write the file to the filesystem */
    int file_size = strlen((uint8_t*)argv[2]);
    fs_file_write((uint8_t*)argv[1], argv[2], file_size);

    printf("Wrote file %s with %d bytes\n", argv[1], file_size);
    return 0;
}