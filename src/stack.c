#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include "calc.h"

void allocStack(Stack* stack) 
{
    assert(stack->capacity > 0);
    stack->top = malloc(stack->capacity * sizeof(Tok));
    
    if (!stack->top) {
        printf("STACK ERROR: Failed allocating the pointer");
    }
}

void push(Stack* stack, Tok tok) 
{
    int index;
    if(stack->count == stack->capacity) {
        stack->capacity *=2;
        stack->top = realloc(stack->top, sizeof(Tok) * stack->capacity); 
        
        if (!stack->top) {
            printf("STACK ERROR: failed reallocation");
            exit(1);
        }
    }

    index = stack->count;
    stack->top[index] = tok;
    stack->count++;
    
    
}

Tok pop(Stack* stack) 
{
    assert(stack->count!=0);

    Tok tok = stack->top[stack->count-1];
    stack->count--;
    return tok;
}

void printStack(Stack* stack) 
{
    for (int i = 0; i < stack->count; i++) {
        Tok tok = stack->top[i];
        printf("----------------- %d -------------------\n", i + 1);
        printf("VALUE: %.2f\nPREC: %d\n", tok.value, tok.prec);
    }
}

Tok peek(Stack* stack) 
{
    return stack->top[stack->count-1];
}

PREC peek_at_prec(Stack* stack) {
    return stack->top[stack->count-1].prec;
}

// returns true if it is empty
bool isEmpty(Stack* stack) 
{
    if (stack->count == 0) return true;
    else return false;
}