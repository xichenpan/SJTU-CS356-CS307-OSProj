#include "buffer.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

/* the buffer */
buffer_item buffer[BUFFER_SIZE];

sem_t empty;
sem_t full;
pthread_mutex_t mutex;
int length;

void buffer_init() {
    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
    length = 0;
}

int insert_item(buffer_item item) {
    sem_wait(&empty);
    pthread_mutex_lock(&mutex);
    buffer[length] = item;
    length++;
    pthread_mutex_unlock(&mutex);
    sem_post(&full);
    return 0;
}

int remove_item(buffer_item *item) {
    sem_wait(&full);
    pthread_mutex_lock(&mutex);
    *item = buffer[0];
    length--;
    for (int i = 0; i < length; i++)
        buffer[i] = buffer[i + 1];
    pthread_mutex_unlock(&mutex);
    sem_post(&empty);
    return 0;
}
