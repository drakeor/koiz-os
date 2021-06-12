#ifndef _INTERRUPT_HANDLER_H_
#define _INTERRUPT_HANDLER_H_

#include <stdint.h>

/**
 * setup_idt()  - Sets up the Interrupr Descriptor Table
 *                Note that this function is called by
 *                load_idt() which actually loads in the interrupts
 */
void setup_idt(void);

void self_test_idt(void);

#endif