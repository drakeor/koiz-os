#ifndef _PIC_H_
#define _PIC_H_

#include <stdint.h>

void PIC_remap();

void PIC_acknowledge(uint8_t interrupt);

void PIC_set_interrupt_masks();

uint8_t read_kb_scan_code(void);

#endif