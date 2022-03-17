#include "tss_entry.h"
#include "tss.h"
#include "../../libc/stdlib.h"
#include "../../libc/string.h"

/* Store the TSS in static memory */
static tss_entry_t TSS;

/* Pointer to the GDT entry in memory */
struct gdt_entry_tss {
    uint16_t limit;
    uint16_t base_0;
    uint8_t base_16_23;
    uint8_t access_byte;
    uint8_t limit_and_flags;
    uint8_t base_24_31; 
} __attribute__((packed));

extern struct gdt_entry_tss gdt_tss;
extern struct gdt_entry_tss gdt_start;
extern struct gdt_entry_tss gdt_data;
extern void tss_flush(void);

void load_tss(void)
{
    // Each segment is 8 bytes. Offset is 5 away

    /* Grab address of TSS and verify */
    uint32_t entryAddrBig = (uint32_t)&TSS;
    printf("TSS Address: %x\n", &entryAddrBig);
    uint16_t entrySize = (uint16_t)sizeof(tss_entry_t);

    /* Print out the GDT TSS Entry stuff */
    printf("GDT TSS Entry Location: %x\n", &gdt_tss);

    /* Update the GDT Entry */
    gdt_tss.limit = entrySize;
    gdt_tss.base_0 = entryAddrBig & 0xFFFF;
    gdt_tss.base_16_23 = (entryAddrBig >> 16) & 0xFF;
    gdt_tss.access_byte = 0x89;
    gdt_tss.limit_and_flags = 0x40;
    gdt_tss.base_24_31 = (entryAddrBig >> 24) & 0xFF;

    /* Print out the new GDT TSS Values */
    printf("New GDT TSS Entry Values: %x | %x\n", 
        gdt_tss.limit,
        gdt_tss.base_16_23
    );

    /* Zero out the TSS */
    memset ((void*) &TSS, 0, sizeof (tss_entry_t));

    /* Set default stack and segments. */
    TSS.ss0  = 0x10;
	TSS.esp0 = 0x0;
	TSS.cs   = 0x0b;
	TSS.ss   = 0x13;
	TSS.es   = 0x13;
	TSS.ds   = 0x13;
	TSS.fs   = 0x13;
	TSS.gs   = 0x13;

    /* giving this a try */
    //_init_gdt();

    /* Load the TSS segment */
    //uint16_t tssSegmentLoc = 8 * 5;
    //uint16_t tssSegmentLoc = 0x2B;
    tss_flush();
}
