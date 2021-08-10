#include "sh_slabinfo.h"

#include "../../libc/malloc.h"

int32_t sh_slabinfo(int argc, char* argv[])
{
    kmemlist();
    return 0;
}