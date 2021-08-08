#include "shell.h"
#include "../libc/stdlib.h"
#include "../libc/string.h"

#define COMMAND_BUFFER_SIZE 1024

static char command_buffer[COMMAND_BUFFER_SIZE];
static int command_buffer_ptr = 0;

static void shell_print_prefix(void)
{
    printf("shell# ");
}

static void shell_execute_command(void)
{
    printf("\n");
    printf("unknown command: %s \n", command_buffer);

    shell_print_prefix();
    command_buffer_ptr = 0;
}

void shell_init(void)
{
    printf("Koiz-OS v1.0 \n");
    shell_print_prefix();
}

void shell_update(void)
{
    /* Grab characters off of standard input */
    char next_char;
    while((next_char = stdlib_pop_stdio_input_char()) != '\0')
    {
        /* special case for enter key */
        if(next_char == 0xA) {
            /* snip off string here */
            command_buffer[command_buffer_ptr] = 0x0;
            ++command_buffer_ptr;
            /* execute command */
            shell_execute_command();
            /* move to next character */
            continue;
        }

        /* otherwise add command to command buffer and print it */
        command_buffer[command_buffer_ptr] = next_char;
        ++command_buffer_ptr;
        printf("%c", next_char);
    }
}