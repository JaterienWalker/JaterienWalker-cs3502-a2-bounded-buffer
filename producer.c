/*
 * producer.c
 * CS 3502 - Assignment 2: Bounded Buffer
 * Author: Walker
 *
 * Usage: ./producer <id> <num_items>
 *
 * Produces num_items into the shared circular buffer using the
 * standard three-semaphore protocol:
 *   wait(empty) -> wait(mutex) -> add item -> post(mutex) -> post(full)
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

    int producer_id = atoi(argv[1]);
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

        int value = producer_id * 1000 + i;

        sem_wait(empty);   /* wait for a free slot */
        sem_wait(mutex);   /* lock the buffer */

        shm->buffer[shm->head].value = value;
        shm->buffer[shm->head].producer_id = producer_id;
        shm->head = (shm->head + 1) % BUFFER_SIZE;
        shm->count = shm->count + 1;

        sem_post(mutex);   /* unlock the buffer */
        sem_post(full);    /* signal a new item is ready */

        printf("Producer %d: Produced value %d\n", producer_id, value);
    }

    /* clean up */
    shmdt(shm);
    sem_close(empty);
    sem_close(full);
    sem_close(mutex);

    return 0;
}
