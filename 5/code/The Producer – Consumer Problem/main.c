#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "buffer.h"
#include <assert.h>
#include <unistd.h>

void *producer(void *param);
void *consumer(void *param);

typedef struct thread {
    pthread_t tid;
    struct thread *next;
} LinkThread;
int sleep_time, producer_num, consumer_num;

int main(int argc, char *argv[]) {

    /* 1. Get command line arguments argv[1],argv[2],argv[3] */
    assert(argc == 4);
    sleep_time = atoi(argv[1]);
    producer_num = atoi(argv[2]);
    consumer_num = atoi(argv[3]);

    /* 2. Initialize buffer */
    buffer_init();
    srand((int)time(0));
    pthread_t producer_thread[producer_num];
    pthread_t consumer_thread[consumer_num];

    /* 3. Create producer thread(s) */
    for (int i = 0; i < producer_num; i++)
        pthread_create(&producer_thread[i], NULL, producer, NULL);

    /* 4. Create consumer thread(s) */
    for (int i = 0; i < consumer_num; i++)
        pthread_create(&consumer_thread[i], NULL, consumer, NULL);

    /* 5. Sleep */
    sleep(sleep_time);

    /* 6. Exit */
    for (int i = 0; i < producer_num; i++)
        pthread_cancel(producer_thread[i]);

    for (int i = 0; i < consumer_num; i++)
        pthread_cancel(consumer_thread[i]);
    return 0;
}

void *producer(void *param) {
    buffer_item item;

    while (1) {
        /* sleep for a random period of time */
        sleep(rand() % sleep_time);
        /* generate a random number */
        item = rand();
        if (insert_item(item))
            printf("report error condition\n");
        else
            printf("producer produced %d\n", item);
    }
}

void *consumer(void *param) {
    buffer_item item;

    while (1) {
        /* sleep for a random period of time */
        sleep(rand() % sleep_time);
        if (remove_item(&item))
            printf("report error condition\n");
        else
            printf("consumer consumed %d\n", item);
    }
}
