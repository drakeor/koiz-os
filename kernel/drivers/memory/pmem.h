#ifndef _PMEM_H_
#define _PMEM_H_

#include "stdint.h"
#include "../../libc/multiboot.h"

/* Size of physical blocks of memory to give out */
#define PHYS_BLOCK_SIZE 4096

/**
 * pmem_alloc() - Allocates a page of physical memory and returns the address
 * 
 * Note that the page given is not guaranteed to be zeroed out
 */
void* pmem_alloc();

/**
 * pmem_free() - Frees a page of physical memory associated with the address
 * 
 * @ptr: pointer to the page to free
 * 
 * Note that if the page is already free, a kernel panic will happen
 */
int pmem_free(void* ptr);

/**
 * pmem_initialize() - Initializes the physical ram handler
 * 
 * This initializes the rest of the free memory outside the reserved area
 * for the kernel. This memory can be used as heap or general memory for
 * user and kernel-space applications.
 * 
 * The strategy is we scan the memory map (populated by GRUB) for the
 * largest section of free blocks. We then setup a bit-map to track
 * the memory that's reserved and mark the rest of the memory as free
 * to give out to other applications
 */
void pmem_initialize(void);

/**
 * pmem_isinit() - Gets the initialization status of pmem
 * 
 * Returns 1 if the physical ram handler is initialized
 * Returns 0 if the physical ram handler is not initialized
 */
int pmem_isinit(void);

/**
 * pmem_set_mbd() - Sets the multiboot info and reserved kernel addr
 * 
 * @mbd: This is the address to the mbd as populated by GRUB
 * @kernel_reserved_end_addr: Address to end of reserved kernel space
 * 
 * Note that this has to be called BEFORE pmem_initialize()
 */
void pmem_set_mbd(multiboot_info_t* mbd, uint32_t kernel_reserved_end_addr);

/**
 * pmem_list_info() - Lists info for the physical memory
 */
void pmem_list_info(void);

#endif