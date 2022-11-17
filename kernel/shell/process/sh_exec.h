#ifndef _SH_EXEC_H_
#define _SH_EXEC_H_

#include <stdint.h>

/**
 * sh_exec() - Executes a process in the foreground
 */
int32_t sh_exec(int argc, char* argv[]);


#endif