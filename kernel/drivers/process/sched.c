#include "sched.h"
#include "process.h"

#include "stdint.h"
#include "stddef.h"

#include "../../config/config.h"
#include "../../libc/stdlib.h"

#define MAX_PRIORITY_QUEUES 3
#define STARTING_TICKETS 50000
#define FULL_TIMESLICE_COST 5

struct mlfq_entry {
    struct process* ptr;
    int priority;
    int tickets_remaining;
    struct mlfq_entry* next_process;
};

/* Since the process list is fixed, we can just used a fixed
    section of memory here versus malloc-ing new memory. It's
    small enough it doesn't matter too much */
struct mlfq_entry proc_list[MAX_PROCESSES];
struct mlfq_entry *proc_priority_list[MAX_PRIORITY_QUEUES] = { 0 };
struct mlfq_entry *current_process = 0;

void sched_init(void)
{
    int i = 0;
    for(i = 0; i < MAX_PRIORITY_QUEUES; i++) {
        proc_priority_list[i] = 0;
    }
    printf("Initialized Process Scheduler\n");
}

void sched_addpid(int pid)
{
    struct process* ptr = process_get(pid);
    if(ptr == NULL) {
        printf("Tried to add PID %d to scheduler but is null process!\n");
        panic("Tried to schedule null process!");
    }

    /* You laugh, but it's so i can reference 
        these fields when I need too easier! */
    proc_list[pid].ptr = ptr;
    proc_list[pid].priority = 0;

    proc_list[pid].tickets_remaining = STARTING_TICKETS;
    
    /* Push it in front of the priority list */
    proc_list[pid].next_process = proc_priority_list[0];
    proc_priority_list[0] = &(proc_list[pid]);

#ifdef DEBUG_MSG_SCHEDULER
    printf("Schduler: Added new PID %d to scheduler. Name: %s \n", 
        pid, ptr->name);
#endif
}

void sched_printinfo(void)
{
    printf("Current Process: ");
    if(current_process != NULL) {
        if(current_process->ptr == NULL) {
            panic("There exists a null process in the scheduler!\n");
        }
        printf("%d\n", current_process->ptr->pid);
    } else {
        printf("<empty>\n");
    }

    int i = 0;
    for(i = 0; i < MAX_PRIORITY_QUEUES; i++) {
        printf("P%d:", i);
        if(proc_priority_list[i] == NULL) {
            printf("<empty>\n");
            continue;
        }
        struct mlfq_entry* cptr = proc_priority_list[i];
        while(cptr != NULL) {
            if(cptr->ptr == NULL) {
                panic("There exists a null process in the scheduler!\n");
            }
            printf(" %d (%d) -> ", cptr->ptr->pid, cptr->tickets_remaining);
            cptr = cptr->next_process;
        }
        printf("\n");
    }
}

void sched_update(void)
{

    /* Decrement the current process by number of tickets 
       (if applicable).
        TODO: If the process yielded it's time beforehand, 
       take that into account */
    if(current_process != NULL) {

        /* Sanity check */
        if(current_process->ptr == NULL) {
            panic("There exists a null process in the scheduler!\n");
        }
        current_process->tickets_remaining -= FULL_TIMESLICE_COST;

        /* Check if we need to bump the current process down
             a priority queue */
        /*if(current_process->tickets_remaining <= 0) {
            (current_process->priority)++;
            current_process->tickets_remaining = STARTING_TICKETS;
            if(current_process->priority >= MAX_PRIORITY_QUEUES)
                current_process->priority = MAX_PRIORITY_QUEUES-1;
        }*/
    }

    /* Okay, now run through our priority queue in order to find
        the next process to run*/
    int i = 0;
    for(i = 0; i < MAX_PRIORITY_QUEUES; i++) {

    }

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