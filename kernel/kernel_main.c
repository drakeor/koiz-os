/* Include free-standing libs */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Include for PORT_COM1 stuff */ 
#include "drivers/serial/serial.h"
#include "drivers/irq/idt_setup.h"
#include "drivers/irq/pic.h"
#include "drivers/memory/pmem.h"

/* Include our standard library */
#include "libc/stdlib.h"
#include "libc/multiboot.h"

/* Include tests */
#include "tests/interrupt_tests.h"
#include "tests/pmem_tests.h"

void kernel_init()
{

    /* Initialize the serial connection right away */
    /* We use this for logging messagse in stdlib.h */
    uint8_t serial_started = serial_init(PORT_COM1);
    if(serial_started != SERIAL_SUCCESS) {
        printf("error initializaing serial port COM1\n");
        panic("cannot initialize serial connection");
        return;
    }
    printf("using serial COM1 for logging serial...\n");

    /* Setup Load our IDT */
    load_and_enable_interrupts();

    /* Test our IDT after loading it */
    test_run_idt();

    /* Load PIC stuff */
    pic_remap();
    pic_set_interrupt_masks();

    /* Start our physical memory allocator */
    pmem_initialize();
    pmem_run_tests();

    
}

void kernel_update(void)
{

}


/* Check if the compiler thinks you are targeting the wrong operating system. */
void kernel_main(multiboot_info_t* mbd, uint32_t magic, uint32_t kernel_memory_end) 
{
    printf("kernel memory end is: %x\n", kernel_memory_end);

    /* Make sure the magic number matches for memory mapping*/
    if(magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        printf("magic number is %x", magic);
        panic("invalid magic number!");
    }
    
    /* Sets multiboot info to drivers that need it */
    pmem_set_mbd(mbd, kernel_memory_end);

    kernel_init();

    printf("Refactored Koiz-OS!");

    while(1) {
        kernel_update();
    }
	__builtin_unreachable();
}