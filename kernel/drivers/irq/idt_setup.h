#ifndef _INTERRUPT_HANDLER_H_
#define _INTERRUPT_HANDLER_H_

#include <stdint.h>

/**
 * setup_idt()  - Sets up the Interrupr Descriptor Table
 *                Note that this function is called by
 *                load_idt() which actually loads in the interrupts
 */
void setup_idt(void);

/**
 * load_and_enable_interrupts() - Loads and enables interrupts
 * 
 * Note that the implementation is done in interrupt_handler.asm
 */
void load_and_enable_interrupts(void);

#endif