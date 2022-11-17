#include "sh_execbg.h"

#include "../../libc/stdlib.h"
#include "../../drivers/process/process.h"

int32_t sh_execbg(int argc, char* argv[])
{
    if(argc < 2) {
        printf("exec requires an argument\n");
        printf("Usage: exec filename\n");
        return 1;
    }
    //process_printlist();
    process_execve((uint8_t*)argv[1], 0, 0);

    printf("not implemented yet.\n");
    return 0;
}