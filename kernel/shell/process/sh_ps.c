#include "sh_ps.h"

#include "../../drivers/process/process.h"

int32_t sh_ps(int argc, char* argv[])
{
    process_printlist();
    return 0;
}