/*

 * producer.c

 * CS 3502 - Assignment 2

 * Author: Jaterien Walker

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

    // attach to the shared memory segment, creating it if this is

    // the first process (producer or consumer) to run

    int
