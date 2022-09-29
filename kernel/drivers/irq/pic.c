#include "pic.h"
#include "../io/basic_io.h"

/* Helpful constants below */
#define PIC1		    0x20		/* IO base address for primary PIC */
#define PIC2		    0xA0		/* IO base address for secondary PIC */
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	    (PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	    (PIC2+1)

#define PIC_EOI		    0x20		/* End-of-interrupt command code */

#define ICW1_ICW4	    0x01		/* ICW4 (not) needed */
#define ICW1_SINGLE	    0x02	    /* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04	    /* Call address interval 4 (8) */
#define ICW1_LEVEL	    0x08	    /* Level triggered (edge) mode */
#define ICW1_INIT	    0x10		/* Initialization - required! */
 
#define ICW4_8086	    0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	    0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08	    /* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C	    /* Buffered mode/master */
#define ICW4_SFNM	    0x10		/* Special fully nested (not) */

/* 
 * These constants are referenced from 
 * https://littleosbook.github.io/#programmable-interrupt-controller-pic
 */
#define PIC_ACK                 0x20
#define PIC1_START_INTERRUPT    0x20
#define PIC2_START_INTERRUPT    0x28
#define PIC2_END_INTERRUPT      PIC2_START_INTERRUPT + 7

/**
 * pic_send_eoi() - Sends an end of interrupt command to the PIC chips
 * @irq:    IRQ number
 * 
 * We use this at the end of an IRQ routine.
 * We issue to both PICs if the IRQ > 8.
 */

void pic_send_eoi(uint8_t irq)
{
    if(irq >= 8)
        io_byte_out(PIC2_COMMAND, PIC_EOI);

    io_byte_out(PIC1_COMMAND, PIC_EOI);
}

void pic_remap()
{
    /* We save the makes so we can restore them at the end of the function */
    //uint8_t pic1_mask = io_byte_in(PIC1_DATA);
    //uint8_t pic2_mask = io_byte_in(PIC2_DATA);

    /* 
     * Starts initialization in cascade mode
     * Initialize both PICs the same way 
     */
    io_byte_out(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    io_byte_out(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    
    /* Set vector offsets for both the leader and follower PICs */
    io_byte_out(PIC1_DATA, PIC1_START_INTERRUPT);
    io_wait();
    io_byte_out(PIC2_DATA, PIC2_START_INTERRUPT);
    io_wait();

    /* Tell the leader PIC that the follower is at IRQ2 (0000 0100b) */
    io_byte_out(PIC1_DATA, 4);
    io_wait();

    /* Tell follower PIC that it's identity is 2 (0000 0010b) */
    io_byte_out(PIC2_DATA, 2);
    io_wait();

    /* Set to 8086 mode */
    io_byte_out(PIC1_DATA, ICW4_8086);
    io_wait();
    io_byte_out(PIC2_DATA, ICW4_8086);
    io_wait();

    /* Restore masks */
    //io_byte_out(PIC1_DATA, pic1_mask);
    //io_byte_out(PIC2_DATA, pic2_mask);
    io_byte_out( PIC1_DATA, 0xf8 ); /* master PIC */
    io_byte_out( PIC2_DATA, 0xff ); /* slave PIC */
}

void pic_set_interrupt_masks()
{
    /* Only listen to:
        - irqs 0 (keyboard interrupt) 
        - irqs 1 (PIT interrupt) 
    */
    io_byte_out( PIC1_DATA, 0b11111100 );
    io_byte_out( PIC2_DATA, 0b11111111 );

    /* Re-enable interrupts */
    asm("sti");
}
/*
uint8_t read_kb_scan_code(void)
{
    return io_byte_in(KBD_DATA_PORT);
}
*/
/*
 * Sends out acknowledgement message from interrupt to
 * the corresponding PIC
 */

void pic_acknowledge(uint8_t interrupt)
{
    if (interrupt < PIC1_START_INTERRUPT || interrupt > PIC2_END_INTERRUPT)
        return;

    if (interrupt < PIC2_START_INTERRUPT) 
        io_byte_out(PIC1, PIC_ACK);
    else
        io_byte_out(PIC2, PIC_ACK);
}