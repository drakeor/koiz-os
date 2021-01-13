#include "mem.h"

#define MIN_PHYS_BLOCK_SIZE 4096
#define MAX_PHYS_BLOCK_ORDER 4
/*
 * 2^0 = 4KB
 * 2^1 = 8KB
 * 2^2 = 16KB
 * 2^3 = 32KB
 */