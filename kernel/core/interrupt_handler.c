#include "interrupt_handler.h"
#include "../libc/stdlib.h"

#define INTERRUPT_COUNT 32

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
extern uintptr_t isr_11;
extern uintptr_t isr_12;
extern uintptr_t isr_13;
extern uintptr_t isr_14;
extern uintptr_t isr_15;
extern uintptr_t isr_16;
extern uintptr_t isr_17;
extern uintptr_t isr_18;
extern uintptr_t isr_19;
extern uintptr_t isr_20;
extern uintptr_t isr_21;
extern uintptr_t isr_22;
extern uintptr_t isr_23;
extern uintptr_t isr_24;
extern uintptr_t isr_25;
extern uintptr_t isr_26;
extern uintptr_t isr_27;
extern uintptr_t isr_28;
extern uintptr_t isr_29;
extern uintptr_t isr_30;
extern uintptr_t isr_31;

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
        case 11:
          isr_addr = (uint32_t)&isr_11;
          break;
        case 12:
          isr_addr = (uint32_t)&isr_12;
          break;
        case 13:
          isr_addr = (uint32_t)&isr_13;
          break;
        case 14:
          isr_addr = (uint32_t)&isr_14;
          break;
        case 15:
          isr_addr = (uint32_t)&isr_15;
          break;
        case 16:
          isr_addr = (uint32_t)&isr_16;
          break;
        case 17:
          isr_addr = (uint32_t)&isr_17;
          break;
        case 18:
          isr_addr = (uint32_t)&isr_18;
          break;
        case 19:
          isr_addr = (uint32_t)&isr_19;
          break;
        case 20:
          isr_addr = (uint32_t)&isr_20;
          break;
        case 21:
          isr_addr = (uint32_t)&isr_21;
          break;
        case 22:
          isr_addr = (uint32_t)&isr_22;
          break;
        case 23:
          isr_addr = (uint32_t)&isr_23;
          break;
        case 24:
          isr_addr = (uint32_t)&isr_24;
          break;
        case 25:
          isr_addr = (uint32_t)&isr_25;
          break;
        case 26:
          isr_addr = (uint32_t)&isr_26;
          break;
        case 27:
          isr_addr = (uint32_t)&isr_27;
          break;
        case 28:
          isr_addr = (uint32_t)&isr_28;
          break;
        case 29:
          isr_addr = (uint32_t)&isr_29;
          break;
        case 30:
          isr_addr = (uint32_t)&isr_30;
          break;
        case 31:
          isr_addr = (uint32_t)&isr_31;
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
 * Self-test a few of the recoverable interrupts to make
 * sure everything is working
 * */
void self_test_idt(void)
{
  printf("Self-testing Interrupts\n");
  asm("INT $1");
  asm("INT $3");
  printf("Self-test Complete\n");
}