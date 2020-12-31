#include "interrupt_handler.h"
#include "../libc/stdlib.h"

/*void handle_interrupt(void)
{
    print("Handling an interrupt!\n");
}*/

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

void interrupt_handler(struct cpu_state cpu, struct stack_state stack, uint32_t interrupt)
{
    
    /*int i;
    for(i = 0; i < 10; i++) {
        printf("Handling an interrupt %d: %x!\n", i,  *((&idt_start)+i));
    }*/
    printf("Handling interrupt %x", interrupt);
}


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

    /* Might have been easier to load it into as a 64-bit and
     * manipulate it but whatever */
    /*idt_size = (uint16_t)idt_info[0] << 8 | idt_info[1];
    idt_start_addr = idt_info[2] << 24 
        | idt_info[3] << 16
        | idt_info[4] << 8
        | idt_info[5];*/
    //idt_size = 

    //printf("IDT Size: %x , Start Address: %x\n", idt_size, idt_start_addr);
    //idt_info = 0x100;
    //printf("IDT Size: %d | %x\n", idt_info, idt_info);
}