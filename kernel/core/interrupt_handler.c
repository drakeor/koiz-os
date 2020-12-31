#include "interrupt_handler.h"
#include "../libc/stdlib.h"


/* Represents an entry in the descriptor table */
struct interrupt_descriptor_t {
  uint16_t offset_low;
  uint16_t selector;
  uint8_t unused;
  uint8_t flags;
  uint16_t offset_high;
} __attribute__((packed));

extern struct interrupt_descriptor_t idt_start;
extern uint32_t idt_info;

extern uintptr_t isr_0;

/*
 * This function populates the interrupt descriptor table
 * It's allocated in assembly
 */
void setup_idt(void)
{
    printf("IDT Info %x!\n", &idt_info);
    
    uint32_t isr_0_addr = (uint32_t)&isr_0;

    printf("ISR_0 location: %x\n", isr_0_addr);

    idt_start.offset_high = (isr_0_addr >> 16) & 0xFFFF;
    idt_start.offset_low = isr_0_addr & 0xFFFF;

    printf("Interrupt 0x0: Offset High/Low: %x %x | Selector: %x | Flags: %x \n", idt_start.offset_high, 
        idt_start.offset_low, idt_start.selector, idt_start.flags);
}