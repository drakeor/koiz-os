
#include "drivers/display.h"
#include "drivers/serial.h"

void main () 
{
    /* Test out our screen print functions */
    clear_screen();
    int i;
    int j;
    for(i = 0; i < 202; i++) {
        print("abcdefghijklmnopqrstuvwxyz0123456789", j % 0xFF);
        j += 19;
    }

    /* Test out our serial port */
    print("\n", 0);
    print("starting serial...", 0);
    uint8_t serial_started = init_serial(PORT_COM1);
    if(serial_started != 0) {
        print("error initializaing serial port COM1\n", 0);
        return;
    }
    write_serial(PORT_COM1, 'E');
    write_serial(PORT_COM1, 'A');
    print("done\n", 0);

    print("booted up", 0);

    while(1) {
        uint8_t data = read_serial(PORT_COM1);
        uint8_t end_data[2] = {
            data, 0x00
        };

        print(end_data, 0);
    }

    print("shouldnt get here", 0);

    /* Done */
}
