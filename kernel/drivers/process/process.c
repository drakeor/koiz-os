#include "process.h"
#include "../../libc/stdlib.h"
#include "../../libc/string.h"
#include "../../fs/fs.h"
#include "../memory/pmem.h"

#define MAX_PROCESSES 64

struct process processes[MAX_PROCESSES] = {0};

// helper function to blank out a process
void process_init_process(char* process_name, int pid)
{
    strcpy(processes[0].name, process_name, PROCESS_NAME_SIZE);

    processes[0].cpu_time_ms = 0;
    processes[0].killed = 0;

    memset(&(processes[0].registers), 0, 
        sizeof(processes[0].registers));

    if(processes[0].process_memory_start != NULL)
        panic("Memory leak for process memory!");

    processes[0].process_memory_start = pmem_alloc();
    processes[0].process_memory_size = PHYS_BLOCK_SIZE;
}

void process_init()
{
    /* Zero out the process list */
    uint32_t i = 0;
    for(i = 0; i < MAX_PROCESSES; i++)
    {
        processes[i].state = UNUSED;
    }

    /* First process is always the idle process */
    process_init_process("Idle", 0);
    processes[0].state = RUNNABLE;
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

#define PRINT_BUFFER_SIZE 64

void process_printlist()
{
    char buf_name[PRINT_BUFFER_SIZE];
    char buf_state[PRINT_BUFFER_SIZE];

    printf("PID | Process Name    | State       | Time  | Memory \n");
    uint32_t i = 0;
    for(i = 0; i < MAX_PROCESSES; i++)
    {
        if(processes[i].state != UNUSED)
        {
            strcpy(buf_name, processes[i].name, PRINT_BUFFER_SIZE);
            strcpy(buf_state, process_state_to_str(processes[i].state), 
                PRINT_BUFFER_SIZE);
            rpad(buf_name, ' ', 16);
            rpad(buf_state, ' ', 12);

            printf("%d | %s | %s | %d | %x\n",
                i,
                buf_name,
                buf_state,
                processes[i].cpu_time_ms,
                processes[i].process_memory_start
            );
        }
    }
    printf("\n");
}

int process_get_next_free_pid()
{
    int i = 1;
    for(i = 1; i < MAX_PROCESSES; i++)
        if(processes[i].state != UNUSED)
            return i;
    return -1;
}

int process_execve(const uint8_t* file_name, 
    uint8_t *const argv[], uint8_t *const envp[])
{

    // Check if the program exists first
    if(!fs_file_exists(file_name))
    {
        printf("Error! Program %s not found!\n", file_name);
        return 0;
    }

    // Sanity check if we have enough room for a spare process
    int pid = process_get_next_free_pid();
    if(pid <= 0)
    {
        printf("No free processes available!\n", file_name);
        return 0;
    }

    // Set up the process
    process_init_process(file_name, pid);

    // Load program into memory


    // LASTLY mark as runnable. 
    // Otherwise the scheduler might pick it up before it's ready.
    processes[pid].state = RUNNABLE;

    return 1;
}

int process_kill(int pid)
{
    // For now, just mark the process as killed.
    processes[pid].killed = 1;
}