#include "shell.h"
#include "../libc/stdlib.h"
#include "../libc/string.h"

/* Include all our commands here */
#include "memory/sh_slabinfo.h"
#include "memory/sh_free.h"

/* Structure for each command */
struct shell_cmd_entry {
    char* shell_cmd_name;
    int32_t (*shell_cmd_func)(int, char*[]);
};
typedef struct shell_cmd_entry shell_cmd_entry_t;

/* Array that holds all the kernel shell commands */
static shell_cmd_entry_t shell_cmds[] = {

    /* Memory commands */
    { "slabinfo",       sh_slabinfo },
    { "free",           sh_free}
};

#define SHELL_CMDS_COUNT (sizeof(shell_cmds) / sizeof(shell_cmd_entry_t))

/* Command buffer size */
#define COMMAND_BUFFER_SIZE 1024

/* Holds the command buffer */
static char command_buffer[COMMAND_BUFFER_SIZE];
static int command_buffer_ptr = 0;

static void shell_print_prefix(void)
{
    printf("shell# ");
}

static void shell_execute_command(void)
{
    printf("\n");
    int i;
    for(i = 0; i < SHELL_CMDS_COUNT; i++) {
        if(strcmp(command_buffer, shell_cmds[i].shell_cmd_name) == 0) {
            (*(shell_cmds[i].shell_cmd_func))(0, NULL);
            goto exec_cmd_exit;
        }
    }

    /* Unknown command */
    printf("unknown command: %s \n", command_buffer);

/* Yeah goto is ugly but w/e. I'll fix it later. 
    Maybe split to another function */
exec_cmd_exit:
    /* Reset command buffer */
    shell_print_prefix();
    command_buffer_ptr = 0;
}

void shell_init(void)
{
    printf("Koiz-OS v1.0 \n");
    printf("Number of commands: %d\n", SHELL_CMDS_COUNT);
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