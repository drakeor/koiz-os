#ifndef _STDLIB_H_
#define _STDLIB_H_

/**
 * panic() - Causes a kernel panic and immediately stops further processing
 * 
 * @message:    panic message to write
 */
void panic(char *message);

/**
 * printf() - Prints a message to the standard display device
 * 
 * @message:    message to write
 */
void printf(char *format, ...);

#endif