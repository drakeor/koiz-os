/* Include free-standing libs */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Include for PORT_COM1 stuff */ 
#include "drivers/serial/serial.h"

/* Include our standard library */
#include "libc/stdlib.h"

void kernel_init(void)
{
    /* Initialize the serial connection right away */
    /* We use this for logging messagse in stdlib.h */
    init_serial(PORT_COM1);
}

void kernel_update(void)
{

}

/* Check if the compiler thinks you are targeting the wrong operating system. */
void kernel_main(void) 
{
    kernel_init();

    printf("Refactored Koiz-OS!");

    while(1) {
        kernel_update();
    }
	__builtin_unreachable();
}