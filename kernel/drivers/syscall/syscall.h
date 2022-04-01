#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#define SYSCALL_TEST	0

/**
 * syscall_handler() - Function that handles syscalls
 * 
 * @port:   Which port to establish the connection. This is typically
 *          one of the PORT defines above
 * 
 * Returns 0 if the serial connection was successful
 * Returns SERIAL_ERROR_CONNECTION_FAILED if connection failed
 */
void syscall_handler(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx);

#endif