#include "shell.h"
#include "../libc/stdlib.h"
#include "../libc/string.h"

/* Include all our commands here */
#include "filesystem/sh_ls.h"
#include "filesystem/sh_write.h"
#include "filesystem/sh_read.h"

#include "memory/sh_slabinfo.h"
#include "memory/sh_free.h"

#include "process/sh_ps.h"
#include "process/sh_exec.h"
#include "process/sh_execbg.h"
#include "process/sh_sched.h"

/* Structure for each command */
struct shell_cmd_entry {
    char* shell_cmd_name;
    int32_t (*shell_cmd_func)(int, char*[]);
};
typedef struct shell_cmd_entry shell_cmd_entry_t;

/* Array that holds all the kernel shell commands */
static shell_cmd_entry_t shell_cmds[] = {

    /* Filesystem commands */
    {"ls",              sh_ls},
    {"write",           sh_write},
    {"read",            sh_read},

    /* Memory commands */
    { "slabinfo",       sh_slabinfo },
    { "free",           sh_free},

    /* Process commands */
    {"ps",              sh_ps},
    {"exec",            sh_exec},
    {"execbg",          sh_execbg},
    {"sched",           sh_sched}
    
};

#define SHELL_CMDS_COUNT (sizeof(shell_cmds) / sizeof(shell_cmd_entry_t))

#define SHELL_MAX_ARGS 50

/* Command buffer size */
#define COMMAND_BUFFER_SIZE 1024

/* Holds the command buffer */
static char command_buffer[COMMAND_BUFFER_SIZE];
static char working_buffer[COMMAND_BUFFER_SIZE];
static int command_buffer_ptr = 0;

static void shell_print_prefix(void)
{
    printf("shell# ");
}

static void shell_execute_command(void)
{
    /* For processing arguments */
    int c_argc = 0;
    int i;
    char* c_argv[SHELL_MAX_ARGS];

    /* This is a really bad implementation. We need to create an FSM
       to process this stuff correctly and keep it concise... */
    int cstr_len = strlen((uint8_t*)command_buffer);
    memcpy(working_buffer, command_buffer, COMMAND_BUFFER_SIZE);
    c_argv[0] = &working_buffer[0];
    c_argc = 1;

    int insideDQuoteState = 0;
    int newStringState = 0;
    
    /* Break up into an array of strings in the working buffer */
    for(i = 0; i < cstr_len; i++)
    {
        if(working_buffer[i] == '\'') {
            insideDQuoteState = !insideDQuoteState;
            continue;
        }
        if(working_buffer[i] == ' ') {
            if(!insideDQuoteState) {
                working_buffer[i] = '\0';
                newStringState = 1;
            }
            continue;
        }
        if(newStringState) {
            c_argv[c_argc] = &working_buffer[i];
            ++c_argc;
            newStringState = 0;
        }
    }

#ifdef DEBUG_MSG_SHELL
    /* Print the arguments and argument positions in debug mode */
    printf("\n");
    for(i = 0; i < c_argc; i++)
    {
        printf("Arg %d: %s\n", i, c_argv[i]);
    }
#endif

    /* Match the first argument to a shell command */
    printf("\n");
    for(i = 0; i < SHELL_CMDS_COUNT; i++) {
        if(strcmp(c_argv[0], shell_cmds[i].shell_cmd_name) == 0) {
            (*(shell_cmds[i].shell_cmd_func))(c_argc, c_argv);
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