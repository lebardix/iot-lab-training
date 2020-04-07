#include <stdio.h>

#include "shell.h"

/* Include threads header and declare the thread stack */
#include "thread.h"
static char stack[THREAD_STACKSIZE_MAIN];

/* Implement thread handler function here */
static void *thread_handler(void *arg)
{
    (void)arg;
    while (1) {}
    return NULL;
}

int main(void)
{

    /* Start new threads here */
    thread_create(stack, sizeof(stack), THREAD_PRIORITY_MAIN + 1,
               0, thread_handler, NULL, "new thread");
    /* Start the shell here */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);
    
    return 0;
}
