#include "tty.h"
#include "display.h"

#define TTY_COMMAND_BUFFER 512

char tty_buffer[TTY_COMMAND_BUFFER];
static int tty_buffer_marker = 0;
static int tty_execute_command = 0;

/*
 * Clears out the terminal and starts up the terminal
 */
void init_tty(void)
{
    int i;

    for(i = 0; i < TTY_COMMAND_BUFFER; i++)
        tty_buffer[i] = 0x00;

    tty_buffer_marker = 0;
    tty_execute_command = 0;

    kprint("@# ", 0xF);
}

int tty_putchar(char ascii_char)
{
    /* ensure the buffer doesn't overflow */
    if(tty_buffer_marker+1 >= TTY_COMMAND_BUFFER)
        return -1;

    /* If the line if a newline, flag the command to be executed */
    if(ascii_char == 0xA) {
        tty_execute_command = 1;
        kprint_char('\n', 0xF);
        return 0;
    }

    /* add character to the buffer and write to the tty */
    tty_buffer[tty_buffer_marker] = ascii_char;
    ++tty_buffer_marker;
    kprint_char(ascii_char, 0xF);

    return 0;
}

int tty_update(void)
{
    /* Handles commands */
    if(tty_execute_command == 1) {
        tty_buffer_marker = 0;
        tty_execute_command = 0;
        kprint("unknown command", 0xF);
        kprint("\n@# ", 0xF);
    }
}
