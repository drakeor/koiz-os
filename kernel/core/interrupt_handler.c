#include "interrupt_handler.h"
#include "../libc/stdlib.h"

#define INTERRUPT_COUNT 10

/* Represents an entry in the descriptor table */
struct interrupt_descriptor_t {
  uint16_t offset_low;
  uint16_t selector;
  uint8_t unused;
  uint8_t flags;
  uint16_t offset_high;
} __attribute__((packed));

extern struct interrupt_descriptor_t idt_start[INTERRUPT_COUNT];

/* For debug */
extern uint32_t idt_info;

/* Specific interrupt handlers */
extern uintptr_t isr_0;
extern uintptr_t isr_1;
extern uintptr_t isr_2;
extern uintptr_t isr_3;
extern uintptr_t isr_4;
extern uintptr_t isr_5;
extern uintptr_t isr_6;
extern uintptr_t isr_7;
extern uintptr_t isr_8;
extern uintptr_t isr_9;
extern uintptr_t isr_10;

/*
 * This function populates the interrupt descriptor table
 * It's allocated in assembly
 */
void setup_idt(void)
{
#ifdef DEBUG_MSG_INTERRUPTS
    printf("IDT Info %x!\n", &idt_info);
#endif

    int i;
    for(i = 0; i < INTERRUPT_COUNT; i++) {

      /* Grab address of ISR routine */
      /* Not the cleanest way, but it works */
      uint32_t isr_addr = 0;
      switch(i) {
        case 0:
          isr_addr = (uint32_t)&isr_0;
          break;
        case 1:
          isr_addr = (uint32_t)&isr_1;
          break;
        case 2:
          isr_addr = (uint32_t)&isr_2;
          break;
        case 3:
          isr_addr = (uint32_t)&isr_3;
          break;
        case 4:
          isr_addr = (uint32_t)&isr_4;
          break;
        case 5:
          isr_addr = (uint32_t)&isr_5;
          break;
        case 6:
          isr_addr = (uint32_t)&isr_6;
          break;
        case 7:
          isr_addr = (uint32_t)&isr_7;
          break;
        case 8:
          isr_addr = (uint32_t)&isr_8;
          break;
        case 9:
          isr_addr = (uint32_t)&isr_9;
          break;
        case 10:
          isr_addr = (uint32_t)&isr_10;
          break;
        default:
          panic("Improperly configured IDT Table!!");
      }

      /* Fill in the IDT table with the proper values */
#ifdef DEBUG_MSG_INTERRUPTS
      printf("ISR %d function location: %x\n", i, isr_addr);
#endif 

      idt_start[i].offset_high = (isr_addr >> 16) & 0xFFFF;
      idt_start[i].offset_low = isr_addr & 0xFFFF;

#ifdef DEBUG_MSG_INTERRUPTS
      printf("Interrupt %d: Offset High/Low: %x %x | Selector: %x | Flags: %x \n", 
        i, idt_start[i].offset_high, idt_start[i].offset_low, idt_start[i].selector, idt_start[i].flags);
#endif 
    
    }
}

/* 
 * Self-test. We can't do this as a loop since the compiler will complain,
 * even with pre-processor macros. Guess we could dynamically do this in assembly..
 * */
void self_test_idt(void)
{
   asm("INT $0");
   asm("INT $1");
   asm("INT $2");
   asm("INT $3");
   asm("INT $4");
   asm("INT $5");
   asm("INT $6");
   asm("INT $7");
   asm("INT $8");
   asm("INT $9");
}