#include "malloc_tests.h"

#include "pmem_tests.h"
#include <stdint.h>
#include "../libc/stdlib.h"
#include "../libc/malloc.h"

#define TEST_MALLOC_SEED 32892
#define MEM_SLOT_COUNT 100
#define MALLOC_ITERATIONS 20
#define MAX_MALLOC_SIZE 1024

void malloc_run_tests()
{
    printf("malloc: running tests...\n");

    /* Simple test */
    void* smallTest = malloc(128);
    free(smallTest);

    printf("Address for smalltest is %x\n", smallTest);

    /* more complicated iteration test */
    int i = 0;
    void* slots[MEM_SLOT_COUNT] = {0};

    /* Each iteration, malloc and free memory in each slot */
    for(i = 0; i < MALLOC_ITERATIONS; i++)
    {
        int j;
        for(j = 0; j < MEM_SLOT_COUNT; j++)
            slots[j] = malloc(maxrand(TEST_MALLOC_SEED, MAX_MALLOC_SIZE));
        for(j = 0; j < MEM_SLOT_COUNT; j++)
            free(slots[j]);
    }
    printf("Iterative tests passed.\n");

    printf("malloc: malloc tests passed...\n");
}
