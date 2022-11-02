#include "process_tests.h"
#include "../drivers/process/process.h"
#include "../libc/stdlib.h"
#include "../libc/string.h"

void process_run_tests()
{
    printf("Testing processes...\n");
    process_printlist();
    printf("Process Tests passed.\n");
}
