#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include "calc.h"

void allocQueue(Queue* queue) 
{
    assert(queue->capacity != 0);
    queue->head_pt = malloc(sizeof(Tok) * queue->capacity);

    if (!queue->head_pt) {
        fprintf(stderr, "QUEUE ERROR: Failed allocating space");
        exit(1);
    }
}

void enqueue(Queue* queue, Tok tok) 
{
    if(queue->tail == queue->capacity) 
    {
        queue->capacity *= 2;
        queue->head_pt = realloc(queue->head_pt, sizeof(Tok) * queue->capacity);
        if (!queue->head_pt) {
            fprintf(stderr, "QUEUE ERROR: failed reallocating memory for head");
            exit(1);
        } 
    }

    queue->head_pt[queue->tail] = tok;   
    queue->tail++;
}


Tok dequeue(Queue* queue) 
{
    assert(queue->tail != 0); // empty queue

    Tok tok = queue->head_pt[queue->head];
    queue->head++;
    
    if (queue->head == queue->tail) {
        queue->tail = 0;
        queue->head = 0;
    }
    return tok;
}