#pragma once

#include <stdbool.h>
#include "calc.h"

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
PREC peek_at_prec(Stack* stack);
bool isEmpty(Stack* stack);
