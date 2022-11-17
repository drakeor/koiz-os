#ifndef _PROCESS_H_
#define _PROCESS_H_

#include "stdint.h"

#define PROCESS_NAME_SIZE 16
#define MAX_PROCESSES 64

struct process_registers
{
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;

    uint32_t esi;
    uint32_t edi;
    uint32_t ebp;

    uint32_t eip;
};

enum process_state { 
    UNUSED      = 0, 
    RUNNABLE, 
    RUNNING, 
    INTERRRUPTABLE_SLEEP, 
    UNINTERRUPTABLE_SLEEP,
    STOPPED,
    ZOMBIE
};

struct process {
    uint8_t name[PROCESS_NAME_SIZE];

    int pid;
    
    uint32_t cpu_time_ms;

    uint32_t current_priority;
    
    enum process_state state;
    int killed;
    struct process_registers registers;

    intptr_t* process_memory_start;
    int32_t process_memory_size;
};

void process_init();

int process_exec_test();

int process_execve(uint8_t* file_name, 
    uint8_t *argv[], uint8_t *envp[]);

int process_kill(int pid);

void process_printlist();

struct process* process_get(int pid);

#endif