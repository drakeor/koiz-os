#ifndef _CONFIG_H_
#define _CONFIG_H_

/* Use serial com1 port as log */
#define USE_COM1_AS_LOG

/* Show debug messages for interrupts */
#define DEBUG_MSG_INTERRUPTS

/* Run tests for interrupts */
#define SELF_TEST_INTERRUPTS

/* Show debug messages for memory maps */
//#define DEBUG_MSG_MEMORYMAP

/* Run tests for pmem. Note that this test takes a while! */
#define SELF_TEST_PMEM

/* Show debug messages for pmem */
#define DEBUG_MSG_PMEM

/* Run tests for vmem */
#define SELF_TEST_VMEM

/* Show debug messages from vmem */
#define DEBUG_MSG_VMEM

/* Run tests for ramdisk */
#define SELF_TEST_RAMDISK

/* Show debug messages for ramdisk */
#define DEBUG_MSG_RAMDISK

#endif