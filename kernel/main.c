
#include "drivers/display.h"

void main () 
{
    clear_screen();
    int i;
    int j;
    for(i = 0; i < 202; i++) {
        print("abcdefghijklmnopqrstuvwxyz0123456789", j % 0xFF);
        j += 19;
    }
    print("fin", 0);
}
