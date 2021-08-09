#include "malloc_tests.h"

#include "pmem_tests.h"
#include <stdint.h>
#include "../libc/stdlib.h"
#include "../libc/malloc.h"

void malloc_run_tests()
{
    printf("malloc: running tests...\n");

    void* smallTest = malloc(128);
    printf("Address for smalltest is %x\n", smallTest);
    free(smallTest);

    printf("malloc: malloc tests passed...\n");
}
