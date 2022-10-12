#ifndef _PROCESS_H_
#define _PROCESS_H_

#include "stdint.h"

#define PROCESS_NAME_SIZE 16

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
    char name[PROCESS_NAME_SIZE];

    uint32_t cpu_time_ms;

    enum process_state state;
    int killed;
    struct process_registers registers;
};

void processes_init();

void process_printlist();

#endif