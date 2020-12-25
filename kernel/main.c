
#include "libc/stdlib.h"

void main () 
{
    kernel_init();

    /* Test out our screen print functions */
    clear_display();
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
