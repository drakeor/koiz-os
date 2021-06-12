#include "interrupt_tests.h"
#include "../libc/stdlib.h"

/* 
 * Self-test a few of the recoverable interrupts to make
 * sure everything is working
 * */
void test_run_idt(void)
{
    printf("Self-testing Interrupts\n");
    asm("INT $1");
    asm("INT $3");
    printf("Self-test Complete\n");
}