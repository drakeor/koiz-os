#include "pit.h"

#include "../io/basic_io.h"

/* 
 * Some hard-coded timer constants
 * Mostly from https://wiki.osdev.org/Programmable_Interval_Timer
 */

#define PIT_IO_CHANNEL0             0x40
#define PIT_IO_CHANNEL1             0x41
#define PIT_IO_CHANNEL2             0x42
#define PIT_IO_COMMAND_REGISTER     0x43

/* 
 * Command register (Copy-pasted from https://wiki.osdev.org/Programmable_Interval_Timer)
 *
 * Bits         Usage
    6 and 7      Select channel :
                    0 0 = Channel 0
                    0 1 = Channel 1
                    1 0 = Channel 2
                    1 1 = Read-back command (8254 only)
    4 and 5      Access mode :
                    0 0 = Latch count value command
                    0 1 = Access mode: lobyte only
                    1 0 = Access mode: hibyte only
                    1 1 = Access mode: lobyte/hibyte
    1 to 3       Operating mode :
                    0 0 0 = Mode 0 (interrupt on terminal count)
                    0 0 1 = Mode 1 (hardware re-triggerable one-shot)
                    0 1 0 = Mode 2 (rate generator)
                    0 1 1 = Mode 3 (square wave generator)
                    1 0 0 = Mode 4 (software triggered strobe)
                    1 0 1 = Mode 5 (hardware triggered strobe)
                    1 1 0 = Mode 2 (rate generator, same as 010b)
                    1 1 1 = Mode 3 (square wave generator, same as 011b)
    0            BCD/Binary mode: 0 = 16-bit binary, 1 = four-digit BCD
 */

void pit_initialize(void)
{

}

void pit_interrupt_handler(void)
{

}