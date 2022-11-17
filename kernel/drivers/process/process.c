#include "process.h"
#include "sched.h"
#include "../../config/config.h"
#include "../../libc/stdlib.h"
#include "../../libc/string.h"
#include "../../fs/fs.h"
#include "../memory/pmem.h"

struct process processes[MAX_PROCESSES] = {0};

// helper function to blank out a process
void process_init_process(uint8_t* process_name, int pid)
{
    strcpy(processes[pid].name, process_name, PROCESS_NAME_SIZE);

    processes[pid].current_priority = 0;
    processes[pid].cpu_time_ms = 0;
    processes[pid].killed = 0;

    memset(&(processes[pid].registers), 0, 
        sizeof(processes[pid].registers));

    if(processes[pid].process_memory_start != NULL)
        panic("Memory leak for process memory!");

    processes[pid].process_memory_start = pmem_alloc();
    processes[pid].process_memory_size = PHYS_BLOCK_SIZE;
}

struct process* process_get(int pid)
{
    if(pid <= 0 || pid >= MAX_PROCESSES)
        return 0;

    if(processes[pid].state == UNUSED)
        return 0;

    return &(processes[pid]);
}

void process_init()
{
    /* Zero out the process list */
    uint32_t i = 0;
    for(i = 0; i < MAX_PROCESSES; i++)
    {
        processes[i].pid = i;
        processes[i].state = UNUSED;
        processes[i].process_memory_start = NULL;
    }

    /* First process (PID 0) is always the idle process */
    uint8_t idle_process_name[] = "Idle";
    process_init_process(idle_process_name, 0);
    processes[0].state = RUNNABLE;
}

uint8_t* process_state_to_str(enum process_state state)
{
    switch(state)
    {
        case UNUSED:
            return (uint8_t*) "Unused";
        case RUNNABLE:
            return (uint8_t*) "Runnable";
        case RUNNING:
            return (uint8_t*) "Running";
        case INTERRRUPTABLE_SLEEP:
            return (uint8_t*) "Interruptable Sleep";
        case UNINTERRUPTABLE_SLEEP:
            return (uint8_t*) "Uninterruptable Sleep";
        case STOPPED:
            return (uint8_t*) "Stopped";
        case ZOMBIE:
            return (uint8_t*) "Zombie";
        default:
            return (uint8_t*) "Unknown";
    }
    return (uint8_t*) "Unknown";

}

#define PRINT_BUFFER_SIZE 64

void process_printlist()
{
    uint8_t buf_name[PRINT_BUFFER_SIZE];
    uint8_t buf_state[PRINT_BUFFER_SIZE];

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
        if(processes[i].state == UNUSED)
            return i;
    return -1;
}

int process_execve(uint8_t* file_name, 
    uint8_t *argv[], uint8_t *envp[])
{

#ifdef DEBUG_MSG_PROCESSES
    printf("Loading process %s \n", file_name);
#endif

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
    // TODO: We need to correctly page the user process later
    // TODO: We need to set memory permissions for it so it can't
    //       access kernel memory
    if(processes[pid].process_memory_start == 0)
    {
        printf("Memory not allocated for process. Stopping.");
        processes[pid].state = UNUSED;
        return 0;
    }

    int res = fs_file_read(file_name, 
        processes[pid].process_memory_start, 0);
    
    if(res != 0)
    {
        printf("Failed to copy process to memory. Stopping");
        processes[pid].state = UNUSED;
    }

#ifdef DEBUG_MSG_PROCESSES
    printf("Loaded process %s to memory with pid %d\n",
        file_name, pid);
#endif

    // LASTLY mark as runnable. 
    processes[pid].state = RUNNABLE;
    sched_addpid(pid);
    
    return 1;
}

int process_kill(int pid)
{
    // For now, just mark the process as killed.
    processes[pid].killed = 1;
    
    return 0;
}