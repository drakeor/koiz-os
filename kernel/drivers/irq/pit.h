/* 
 * This is for the programmable interval timer. 
 * Resources: http://www.osdever.net/bkerndev/Docs/pit.htm
 * https://wiki.osdev.org/Programmable_Interval_Timer
 * 
 */



#ifndef _PIT_H_
#define _PIT_H_

// sets up the pit
void pit_initialize(void);

// Interrupt handler whenever the programmable interrupt timer is called
void pit_interrupt_handler(void);


#endif