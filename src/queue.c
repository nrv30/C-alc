#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include "queue.h"
#include "calc.h"

void allocQueue(Queue* queue) 
{
    assert(queue->capacity != 0);
    queue->head_pt = malloc(sizeof(Tok) * queue->capacity);

    if (!queue->head_pt) {
        printf("QUEUE ERROR: Failed allocating space");
        exit(1);
    }
}

void enQueue(Queue* queue, Tok tok) 
{
    if(queue->tail == queue->capacity) 
    {
        queue->capacity *= 2;
        queue->head_pt = realloc(queue->head_pt, sizeof(Tok) * queue->capacity);
        if (!queue->head_pt) {
            printf("QUEUE ERROR: failed reallocating memory for head");
            exit(1);
        } 
    }

    queue->head_pt[queue->tail] = tok;   
    queue->tail++;
}


Tok deQueue(Queue* queue) 
{
    assert(queue->tail != 0); // empty queue

    Tok tok = queue->head_pt[queue->head];
    queue->head++;
    
    if (queue->head == queue->tail-1) {
        queue->tail = 0;
        queue->head = 0;
    }
    return tok;
}

void printQueue(Queue* queue) 
{
    for (int i = 0; i < queue->tail; i++) {
        Tok tok = queue->head_pt[i];
        printf("----------------- %d -------------------\n", i + 1);
        printf("ID: %s\nVALUE: %.2f\nPREC: %d\n", ID_table[tok.id], tok.value, tok.prec);
    }
}