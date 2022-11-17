#ifndef _CONFIG_H_
#define _CONFIG_H_

/* Display debug messages from the interrupt handler */
#define DEBUG_MSG_INTERRUPTS

/* Show debug messages for memory maps */
#define DEBUG_MSG_MEMORYMAP

/* Show debug messages for physical memory */
//#define DEBUG_MSG_PMEM 

/* Show debug messages for virtual memory */
#define DEBUG_MSG_VMEM

/* Show debug messages for the ramdisk FAT16 filesystem */
#define DEBUG_MSG_RAMDISK_FAT16

/* Show debug messages for shell */
#define DEBUG_MSG_SHELL

/* Show debug messages for processes */
#define DEBUG_MSG_PROCESSES

/* Show debug messages for the scheduler */
#define DEBUG_MSG_SCHEDULER

/* Tests the allocations for physical memories */
#define DEBUG_SELF_TEST_PMEM_ALLOCATIONS

/* Tests heap allocations */
//#define DEBUG_SELF_TEST_MALLOC_ALLOCATIONS

#endif