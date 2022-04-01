#include "malloc_tests.h"

#include "pmem_tests.h"
#include <stdint.h>
#include "../libc/stdlib.h"
#include "../libc/malloc.h"

#define TEST_MALLOC_SEED 32892
#define MEM_SLOT_COUNT 1000
#define MALLOC_ITERATIONS 5
#define MAX_MALLOC_SIZE 4096

void malloc_run_tests()
{
#ifdef DEBUG_SELF_TEST_MALLOC_ALLOCATIONS
    printf("malloc: running tests...\n");

    /* Simple test to test maximum malloc bounds*/
    void* smallTest = malloc(MAX_MALLOC_SIZE);
    free(smallTest);

    printf("Address for maximum test is %x\n", smallTest);

    /* A more complicated iteration test */
    int i = 0;
    void* slots[MEM_SLOT_COUNT] = {0};

    /* Each iteration, malloc and free memory in each slot */
    for(i = 0; i < MALLOC_ITERATIONS; i++)
    {

        /* Allocate memory to each slot */
        int j;
        for(j = 0; j < MEM_SLOT_COUNT; j++) {
             
             /*
              * Since the bigger buckets are smaller, distribute more
              * mallocs towards the lower half 
              */
            int max_malloc_size = MAX_MALLOC_SIZE;
            int k;
            for(k = 0; k < 5; k++) {
                if(maxrand(TEST_MALLOC_SEED, 2) < 2)
                    max_malloc_size /= 2;
            }
            slots[j] = malloc(maxrand(TEST_MALLOC_SEED, max_malloc_size));
        }

        /* Free memory in each slot */
        for(j = 0; j < MEM_SLOT_COUNT; j++)
            free(slots[j]);
    }
    printf("Iterative tests passed.\n");

    printf("malloc: malloc tests passed...\n");
#endif

}
