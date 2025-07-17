#include <stdbool.h>
#include "../include/calc.h"

#pragma once
typedef struct Queue
{
    int tail;
    int head;
    int capacity;
    Tok* head_pt;
} Queue;

void allocQueue(Queue* queue);
void enQueue(Queue* queue, Tok tok);
void resizeQueue(Queue* queue);
Tok deQueue(Queue* queue);
void printQueue(Queue* queue);