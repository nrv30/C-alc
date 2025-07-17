#include <stdbool.h>
#include "../include/calc.h"

#pragma once
typedef struct 
{
    int count;
    int capacity;
    Tok* top;
} Stack;

void allocStack(Stack* stack);
void push(Stack* stack, Tok op);
Tok pop(Stack* stack);
void printStack(Stack* stack);
Tok peek(Stack* stack);
bool isEmpty(Stack* stack);
