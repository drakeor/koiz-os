#include "sched.h"
#include "process.h"

#include "../../libc/stdlib.h"

void sched_init(void)
{
    printf("Initialized Process Scheduler\n");
}

void sched_update(void)
{
    /* If the running process is marked as killed, 
        switch to IDLE process and zombie it.*/
    
    /* Update running process */

    /* Increment the CPU time from the 
            last time with interrupt count */

    /* After a certain condition is met, 
            flag to switch tasks */

    /* Loop thru the processes we have */

    /* If we find a runnable to switch to and flag is set,
        then switch to that task*/

    /* Reap zombie processes at this point */
}