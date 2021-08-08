#include "stdio.h"

#include "../drivers/memory/pmem.h"

int io_buffer_empty(stdio_buffer_t* in_buff)
{
    return (in_buff->head_ptr == in_buff->tail_ptr);
}

void io_buffer_place(stdio_buffer_t* in_buff, char* message) 
{
    /* Lazy initialize buffer */
    if(!in_buff->is_init) {
        in_buff->b_addr = pmem_alloc();
        in_buff->b_size = PHYS_BLOCK_SIZE;
        in_buff->is_init = 1;
    }

    /* Go through message and place in buffer */
    int i = 0;
    uint8_t* b_addr = in_buff->b_addr;
    while(message[i] != '\0')
    {
        /* Copy character to buffer */
        b_addr[in_buff->head_ptr] = message[i];
        in_buff->head_ptr = (in_buff->head_ptr + 1) % in_buff->b_size;
        i++;

        /* If we hit our tail_ptr, we looped around and this
           means we overflowed our buffer. For now, we'll throw an error. */
        /* If this is in userspace from a function call, we'd likely switch
           to another process and hopefully consume data */
        /* Likely undo the write too (set head back one) */
        if(in_buff->head_ptr == in_buff->tail_ptr)
            panic("StdLib IO Buffer overflow!");
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