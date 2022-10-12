#include "process.h"
#include "../../libc/stdlib.h"
#include "../../libc/string.h"

#define MAX_PROCESSES 64

struct process processes[MAX_PROCESSES] = {0};

void processes_init()
{
    /* Zero out the process list */
    uint32_t i = 0;
    for(i = 0; i < MAX_PROCESSES; i++)
    {
        processes[i].state = UNUSED;
    }

    /* First process is always the idle process */
    strcpy(processes[0].name, "Idle", PROCESS_NAME_SIZE);
    processes[0].state = RUNNABLE;
    processes[0].cpu_time_ms = 0;
    processes[0].killed = 0;
}

const char* process_state_to_str(enum process_state state)
{
    switch(state)
    {
        case UNUSED:
            return "Unused";
        case RUNNABLE:
            return "Runnable";
        case RUNNING:
            return "Running";
        case INTERRRUPTABLE_SLEEP:
            return "Interruptable Sleep";
        case UNINTERRUPTABLE_SLEEP:
            return "Uninterruptable Sleep";
        case STOPPED:
            return "Stopped";
        case ZOMBIE:
            return "Zombie";
        default:
            return "Unknown";
    }
    return "Unknown";

}
void process_printlist()
{
    printf("PID | Process Name    | Time   | State  |\n");
    uint32_t i = 0;
    for(i = 0; i < MAX_PROCESSES; i++)
    {
        if(processes[i].state != UNUSED)
        {
            printf("%d | %s | %d | %s\n",
                i,
                processes[i].name,
                processes[i].cpu_time_ms,
                process_state_to_str(processes[i].state)
            );
        }
    }
    printf("\n");
}