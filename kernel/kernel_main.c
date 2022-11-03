/* Include free-standing libs */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Include our drivers */ 
#include "drivers/serial/serial.h"
#include "drivers/irq/idt_setup.h"
#include "drivers/irq/pic.h"
#include "drivers/irq/pit.h"
#include "drivers/memory/pmem.h"
#include "drivers/memory/vmem.h"
#include "drivers/process/process.h"
#include "drivers/process/sched.h"
#include "drivers/ramdisk/ramdisk.h"
#include "drivers/tss/tss.h"

/* Include our filesystem stuff */
#include "fs/ramdisk_fat16.h"
#include "fs/fs.h"

/* Include our standard library */
#include "libc/stdlib.h"
#include "libc/multiboot.h"
#include "libc/malloc.h"

/* Include tests */
#include "tests/interrupt_tests.h"
#include "tests/pit_tests.h"
#include "tests/pmem_tests.h"
#include "tests/process_tests.h"
#include "tests/vmem_tests.h"
#include "tests/ramdisk_tests.h"
#include "tests/ramdisk_fat16_tests.h"
#include "tests/malloc_tests.h"

/* Include our shell */
#include "shell/shell.h"

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

    /* Load PIT stuff */
    pit_initialize();
    pit_run_tests();

    /* Start our physical memory allocator */
    pmem_initialize();
    pmem_run_tests();

    /* Enable paging and virtual memory */
    vmem_init();
    vmem_run_tests();
    
    /* Run the tests BEFORE initializing the ramdisk! */
    ramdisk_run_tests();

    /* Run the tests for the temp filesystem BEFORE initializing it! */
    ramdisk_fat16_run_tests();

    /* initialize our temp filesystem */
    ramdisk_fat16_init();

    /* Load starting files */
    fs_load_starting_files();

    /* Run malloc tests */
    malloc_run_tests();

    /* List malloc memory stuff */
    kmemlist();
    
    /* Load up the TSS */
    load_tss();

    /* Load processes */
    process_init();
    process_run_tests();

    /* Load and run scheduler */
    sched_init();

    printf("kernel init complete\n");
}

void kernel_update(void)
{
    /* standard lib processes tasks */
    stdlib_update();

    /* shell process update */
    shell_update();

    /* process scheduler update */
    sched_update();
}

extern void _enter_usermode(void);

/* Main kernel entry point */
void kernel_main(multiboot_info_t* mbd, uint32_t magic, 
    uint32_t kernel_memory_end) 
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

    shell_init();

    /* Flush at this point */
    stdlib_flushstdio();

    _enter_usermode();

    panic("shouldn't reach here?");
    
    while(1) {
        kernel_update();
    }
	__builtin_unreachable(); 
}