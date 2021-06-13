#ifndef _VMEM_H_
#define _VMEM_H_

/**
 * vmem_init() - Initializes paging for the kernel
 * 
 * This will automatically populate the page directory structure
 * as well as the page tables and enable them for the kernel
 */
void vmem_init(void);

#endif