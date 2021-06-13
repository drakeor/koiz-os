#ifndef _INTERRUPT_TESTS_H_
#define _INTERRUPT_TESTS_H_

#include <stdint.h>

/**
 * test_run_idt()    - Tests a few recoverable interrupts
 *                    to make sure the kernel doesn't panic
 */
void test_run_idt(void);

#endif