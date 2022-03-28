#ifndef _STDIO_H_
#define _STDIO_H_

/* Structure for our standard buffers. 
   We're using just a simple ring buffer for standard streams. */
struct stdio_buffer {
    int is_initializing;
    int is_init;
    void* b_addr;
    int head_ptr;
    int tail_ptr;
    int b_size;
    int autoflush_to_screen;
};
typedef struct stdio_buffer stdio_buffer_t;

/**
 * io_buffer_place() - Places a message into the IO ring buffer
 * 
 * @in_buff:    Pointer to the IO Buffer
 * @message:    Message to place in the buffer
 * 
 * Note that if the buffer is not initialized, this will automatically
 * lazy-initialize the buffer and allocate memory for it
 */
void io_buffer_place(stdio_buffer_t* in_buff, char* message);

/**
 * io_buffer_empty() - Returns whether or not the buffer is empty
 * 
 * @in_buff:    Pointer to the IO Buffer
 * 
 * Returns 1 if the buffer is empty. Assumes buffer is initialized.
 */
int io_buffer_empty(stdio_buffer_t* in_buff);

/**
 * io_buffer_pop() - Pops a character off the IO buffer
 * 
 * @in_buff:    Pointer to the IO Buffer
 * 
 * Returns 0 if there's nothing in the buffer
 */
char io_buffer_pop(stdio_buffer_t* in_buff);

/**
 * io_buffer_flush_to_screen() - Flush everything in the buffer onto the screen
 * 
 * Flushes everything currently in the output buffer to screen.
 */
void io_buffer_flush_to_screen(stdio_buffer_t* in_buff);

#endif