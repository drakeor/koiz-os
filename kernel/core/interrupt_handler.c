#include "interrupt_handler.h"
#include "../libc/stdlib.h"

void handle_interrupt(void)
{
    print("Handling an interrupt!\n");
}

/*
 * This function populates the interrupt descriptor table
 * It's allocated in assembly
 */
void setup_idt(void)
{
    uint16_t idt_size;
    uint32_t idt_start_addr;
    
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