#ifndef _INTERRUPT_HANDLER_H_
#define _INTERRUPT_HANDLER_H_

#include <stdint.h>

//void handle_interrupt(void);

//void setup_idt(void);

//extern uint8_t idt_info[6];
//extern uint64_t idt_info[256];

/* 
 * Source: Little OS Book
 * At the time of the interrupts,
 * the stack will look like this:
 * [esp+12] eflags
 * [esp+8] cs
 * [esp+4] eip
 * [esp] error_code
 */

struct cpu_state {
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
    uint32_t esi;
    uint32_t edi;
    uint32_t esp;
} __attribute__((packed));


struct stack_state {
    uint32_t error_code;
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
} __attribute__((packed));


void interrupt_handler(struct cpu_state cpu, struct stack_state stack, uint32_t interrupt);

#endif