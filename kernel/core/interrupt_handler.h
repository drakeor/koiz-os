#ifndef _INTERRUPT_HANDLER_H_
#define _INTERRUPT_HANDLER_H_

#include <stdint.h>

void handle_interrupt(void);

void setup_idt(void);

//extern uint8_t idt_info[6];
//extern uint64_t idt_info[256];

#endif