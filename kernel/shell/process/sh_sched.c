#include "sh_sched.h"

#include "../../drivers/process/sched.h"

int32_t sh_sched(int argc, char* argv[])
{
    sched_printinfo();
    return 0;
}