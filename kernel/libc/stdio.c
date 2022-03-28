#include "stdlib.h"
#include "stdio.h"

#include "../drivers/memory/pmem.h"
#include "../drivers/video/vga.h"

int io_buffer_empty(stdio_buffer_t* in_buff)
{
    return (in_buff->head_ptr == in_buff->tail_ptr);
}

void io_buffer_place(stdio_buffer_t* in_buff, char* message) 
{
    /* Lazy initialize buffer */
    if(!in_buff->is_init) {

        /* Prevent infinite loop bug */
        if(in_buff->is_initializing) {
            panic("io_buffer_place infinite loop detected!");
        }

        in_buff->is_initializing = 1;

        in_buff->b_addr = pmem_alloc();
        in_buff->b_size = PHYS_BLOCK_SIZE;
        in_buff->is_init = 1;

        in_buff->is_initializing = 0;
    }

    /* Go through message and place in buffer */
    int i = 0;
    uint8_t* b_addr = in_buff->b_addr;
    while(message[i] != '\0')
    {
        /* Lookup the next buffer entry */
        int next_head_ptr = (in_buff->head_ptr + 1) % in_buff->b_size;

        /* Use the look-ahead to see if we're about to overflow */
        if(next_head_ptr == in_buff->tail_ptr)
        {
             /* If we autoflush to screen, call that immediately to prevent overflow */
             /* This should push the tail further away so we don't overflow */
            if(in_buff->autoflush_to_screen)
            {
                io_buffer_flush_to_screen(in_buff);
            } 

            /* TODO: If this is in userspace from a function call, we'd likely switch
                to another process and hopefully consume data */
        }

        /* Sanity checks */
        /* Make sure the state hasn't changed otherwise we're in trouble */
        if(next_head_ptr != (in_buff->head_ptr + 1) % in_buff->b_size)
        {
            panic("next_head_ptr mutated unexpectedly");
        }

        /* Copy character to buffer */
        b_addr[in_buff->head_ptr] = message[i];
        in_buff->head_ptr = next_head_ptr;
        i++;

        /* If we hit our tail_ptr, we looped around and this
           means we overflowed our buffer. At this point we can't remedy it.
           So we'll throw an error. */

        if(in_buff->head_ptr == in_buff->tail_ptr)
        {
            panic("StdLib IO Buffer overflow! TODO: Flush what we have at this point automatically! Log to overflows! ");
        }
    }
}

void io_buffer_flush_to_screen(stdio_buffer_t* in_buff)
{
    uint8_t next_char = io_buffer_pop(in_buff);
    while(next_char != '\0')
    {
        vga_print_screen_char(next_char, DEFAULT_TEXT_COLOR);
        next_char = io_buffer_pop(in_buff);
    }
}

char io_buffer_pop(stdio_buffer_t* in_buff)
{
    if(!in_buff->is_init)
        return 0;
        
    if(io_buffer_empty(in_buff))
        return 0;

    char* b_addr = in_buff->b_addr;
    char popped_char =  b_addr[in_buff->tail_ptr];
    in_buff->tail_ptr = (in_buff->tail_ptr + 1) % in_buff->b_size;

    return popped_char;
}