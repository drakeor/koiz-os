#ifndef _PIC_H_
#define _PIC_H_

#include <stdint.h>


 /* 
  * This port is referenced from 
  * https://littleosbook.github.io/#programmable-interrupt-controller-pic
  */
#define KBD_DATA_PORT   0x60

/**
 * pic_remap() - Remap the PIC using the code for 8259 PIC Chips
 * 
 * Reference: https://wiki.osdev.org/PIC
 */
void pic_remap(void);

/**
 * pic_acknowledge() - Sends out an acknowledgement message from
 *                     the interrupt to a corresponding PIC
 * 
 * @interrupt:  Interrupt between PIC1_START_INTERRUPT and PIC1_START_INTERRUPT
 */
void pic_acknowledge(uint8_t interrupt);

/**
 * pic_set_interrupt_masks() - Sets interrupt masks for the PIC
 */
void pic_set_interrupt_masks(void);

#endif