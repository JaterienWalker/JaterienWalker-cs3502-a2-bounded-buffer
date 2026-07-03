/*
 * buffer.h
 * CS 3502 - Assignment 2: Bounded Buffer
 * Author: Jaterien Walker
 */

#ifndef BUFFER_H
#define BUFFER_H

#define BUFFER_SIZE 10
#define SHM_KEY 0x1234

/* item stored */
typedef struct {
    int value;
    int producer_id;
} item_t;

/* the shared circular buffer */
typedef struct {
    item_t buffer[BUFFER_SIZE];
    int head;   /* next write position */
    int tail;   /* next read position */
    int count;  /* number of items currently in the buffer */
} shared_buffer_t;

#endif
