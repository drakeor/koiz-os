#ifndef _STDLIB_H_
#define _STDLIB_H_

void kernel_init();

void clear_display();

void print(char *message);
void error(char *message);

void panic(char *message);

void printf(char *format, ...);

#endif