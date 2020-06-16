/**
 * Example client program that uses thread pool.
 */

#include <stdio.h>
#include <unistd.h>
#include "threadpool.h"

#define QUEUE_SIZE 10

struct data
{
    int a;
    int b;
};

void add(void *param)
{
    struct data *temp;
    temp = (struct data*)param;

    printf("I add two values %d and %d result = %d\n",temp->a, temp->b, temp->a + temp->b);
}

int main(void)
{
    // create some work to do
    struct data work[2 * QUEUE_SIZE];
    pool_init();
    for (int i = 0; i < 2 * QUEUE_SIZE; i++)
    {
        work[i].a = i;
        work[i].b = 2 * i;
        pool_submit(&add, &work[i]);
    }
    sleep(1);
    pool_shutdown();
    return 0;
}
