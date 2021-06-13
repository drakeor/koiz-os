#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

/**
 * keyboard_interrupt_handler() - Handles interrupts for the keyboard
 * 
 * Note that by default, the interrupt for the keyboard is 0x21.
 * When we enter this function, we're in an interrupt
 * This function converts a scan code to character and places it in
 * standard input
 */
void keyboard_interrupt_handler(void);

#endif