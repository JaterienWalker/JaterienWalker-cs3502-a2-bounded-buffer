// buffer.h
// CS 3502 - Assignment 2: Bounded Buffer
// Jaterien Walker

#ifndef BUFFER_H
#define BUFFER_H

#define BUFFER_SIZE 10
#define SHM_KEY 0x1234

typedef struct { int value; int producer_id; } item_t;

typedef struct {
    item_t buffer[BUFFER_SIZE];
    int head, tail, count;
} shared_buffer_t;

#endif
