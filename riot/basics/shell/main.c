#include <stdio.h>

/* Include the shell header */
#include "shell.h"

/* Implement the shell function callback here */
static int _board_handler(int argc, char **argv)
{
    /* These parameters are not used, avoid a warning during build */
    (void)argc;
    (void)argv;

    puts(RIOT_BOARD);

    return 0;
}

static int _cpu_handler(int argc, char **argv)
{
    /* These parameters are not used, avoid a warning during build */
    (void)argc;
    (void)argv;

    puts(RIOT_CPU);

    return 0;
}
/* Add the shell command to the list of commands here */

static const shell_command_t shell_commands[] = {
    { "board", "Print the board name", _board_handler },
    { "cpu", "Print the cpu name", _cpu_handler },
    { NULL, NULL, NULL }
};


int main(void)
{
    /* Start the shell here */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
    return 0;
}
