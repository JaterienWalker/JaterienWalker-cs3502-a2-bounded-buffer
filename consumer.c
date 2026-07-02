/*
 * consumer.c
 * CS 3502 - Assignment 2: Bounded Buffer
 * Author: Walker
 *
 * Usage: ./consumer <id> <num_items>
 *
 * Consumes num_items from the shared circular buffer using the
 * standard three-semaphore protocol:
 *   wait(full) -> wait(mutex) -> remove item -> post(mutex) -> post(empty)
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>

#include "buffer.h"

int main(int argc, char *argv[]) {

    if (argc != 3) {
        printf("Usage: %s <id> <num_items>\n", argv[0]);
        return 1;
    }

    int consumer_id = atoi(argv[1]);
    int num_items = atoi(argv[2]);

    /* get shared memory (create it if it's not there yet) */
    int shm_id = shmget(SHM_KEY, sizeof(shared_buffer_t), IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("shmget failed");
        return 1;
    }

    shared_buffer_t *shm = (shared_buffer_t *) shmat(shm_id, NULL, 0);
    if (shm == (void *) -1) {
        perror("shmat failed");
        return 1;
    }

    /* open the three semaphores (create them if this is the first process) */
    sem_t *empty = sem_open("/sem_empty", O_CREAT, 0644, BUFFER_SIZE);
    sem_t *full  = sem_open("/sem_full",  O_CREAT, 0644, 0);
    sem_t *mutex = sem_open("/sem_mutex", O_CREAT, 0644, 1);

    if (empty == SEM_FAILED || full == SEM_FAILED || mutex == SEM_FAILED) {
        perror("sem_open failed");
        return 1;
    }

    for (int i = 0; i < num_items; i++) {

        sem_wait(full);    /* wait for an item to be ready */
        sem_wait(mutex);   /* lock the buffer */

        int value = shm->buffer[shm->tail].value;
        int from_producer = shm->buffer[shm->tail].producer_id;
        shm->tail = (shm->tail + 1) % BUFFER_SIZE;
        shm->count = shm->count - 1;

        sem_post(mutex);   /* unlock the buffer */
        sem_post(empty);   /* signal a slot is free */

        printf("Consumer %d: Consumed value %d from Producer %d\n",
               consumer_id, value, from_producer);
    }

    /* clean up */
    shmdt(shm);
    sem_close(empty);
    sem_close(full);
    sem_close(mutex);

    return 0;
}
