#include "sh_free.h"

#include "../../drivers/memory/pmem.h"

int32_t sh_free(int argc, char* argv[])
{
    pmem_list_info();
    return 0;
}