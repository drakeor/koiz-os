#include "pit_tests.h"
#include "../libc/stdlib.h"

extern int pit_interrupt_count;

#define OPERATION_TEST_AMOUNT 20000

/* 
 * Iterative factorial as recursive for large
 * numbers might overflow the kernel stack
 */
uint32_t factorial(uint32_t n)
{
    int res = 1;
    int i = 0;
    for(i = 2; i <= n; i++)
        res *= i;
    return res;
}

void pit_run_tests()
{
    
    printf("Testing Timer Interrupt...\n");

    /* Do a O(n^2) operation to busy the CPU */
    int i = 0;
    for(i = 0; i <= OPERATION_TEST_AMOUNT; i++) {
        factorial(i);
        if(i*10 % OPERATION_TEST_AMOUNT == 0)
            printf("Wasting Time... %d/%d\n", 
                i*10 / OPERATION_TEST_AMOUNT, 10);
    }

    /* Check interrupts */
    if(pit_interrupt_count == 0)
        panic("Timer interrupt was never called!");

    printf("Timer Interrupt Count: %d\n", pit_interrupt_count);
    printf("Test Complete!\n");
}