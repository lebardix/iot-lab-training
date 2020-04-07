#include <stdio.h>

#include "thread.h"

/* Add  xtimer include here */
#include "xtimer.h"

/* Add your thread stack and handler function here */
static char stack[THREAD_STACKSIZE_MAIN];
static void *thread_handler(void *arg)
{
    int counter = 0;

    while (1) {
        counter++;
        printf("Message %d from thread '%s'\n", counter, (char *)arg);
        /* Wait one second */
        xtimer_sleep(1);
    }

    return NULL;
}

int main(void)
{
    /* Create the thread here */
    thread_create(stack, sizeof(stack), THREAD_PRIORITY_MAIN - 1,
               0, thread_handler, "my thread", "thread");
    /* Print main thread messages in a loop here, with 500ms delay between each message */
    int counter = 0;
    while (1) {
        counter++;
        printf("Message %d from main thread\n", counter);

        /* add 500ms delay */
        xtimer_usleep(500 * US_PER_MS);
    }
    return 0;
}
