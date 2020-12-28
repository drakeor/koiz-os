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
    print("setting up IDT!\n");
}