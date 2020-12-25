
#include "drivers/display.h"
#include "drivers/serial.h"

#include "libc/stdlib.h"

#include "config.h"

void kernel_init()
{
#if USE_COM1_AS_LOG == 1
    /* Test out our serial port */
    uint8_t serial_started = init_serial(PORT_COM1);
    if(serial_started != 0) {
        print("error initializaing serial port COM1\n");
        return;
    }
    print("using serial COM1 for logging serial...\n");
    //write_serial_string(PORT_COM1, "using serial COM1 for logging\n");
#endif
}

void main () 
{
    kernel_init();

    /* Test out our screen print functions */
    clear_screen();
    int i;
    int j;
    for(i = 0; i < 202; i++) {
        print("abcdefghijklmnopqrstuvwxyz0123456789");
        j += 19;
    }

    print("\n");

    print("booted up");
    error("this is an error test");

   // print("shouldnt get here", 0);

    /* Done */
}
