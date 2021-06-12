#ifndef _INTERRUPT_HANDLER_H_
#define _INTERRUPT_HANDLER_H_

#include <stdint.h>
#include "../config.h"

void setup_idt(void);

void self_test_idt(void);

#endif