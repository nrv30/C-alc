#include <stdio.h>
#include <stdbool.h> 
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "stack.h"
#include "queue.h"

// turn equation to postfix
int make_OutputQueue(Stack* op_stack, Queue* outQueue); // returns number of tokens or -1 if fails
int getPrec(char* tok);
void cleanStack(Stack* op_stack, Queue* outQueue);

// evalueate expression
double evaluate_postfix(Stack* eval_stack, Queue* outQueue);
double comp_expression(char* op, char* first, char* last);
void compare_operators(int prec, Stack* op_stack, Queue* outQueue);


int main(void) 
{
    Stack op_stack;
    op_stack.capacity = 5;
    allocStack(&op_stack);

    Queue outQueue;
    outQueue.capacity = 5;
    allocQueue(&outQueue);

    const int minLen = 3; // smallest equation
    int tokCount = 0;
    int loopCount = 0; 
    do {
        if (loopCount > 0 && tokCount != 0) {
            printf("invalid input re-enter\n");
        } else if (loopCount > 0 && tokCount == 0) {
            return 0;
        }
        loopCount++;
        op_stack.count = -1;
        outQueue.head = -1;
        outQueue.tail = -1;
        tokCount = make_OutputQueue(&op_stack, &outQueue); 
    } while (tokCount < minLen || tokCount == -1 || tokCount == 0);
    
    freeStack(&op_stack);
    // freeQueue(&outQueue);
    
    Stack eval_stack;
    eval_stack.count = -1;
    eval_stack.capacity = 5;
    allocStack(&eval_stack);
    
    double ans = evaluate_postfix(&eval_stack, &outQueue);
    
    freeQueue(&outQueue);
    freeStack(&eval_stack);
    
    printf("%f\n", ans);
    printf(">>\n");
    
    return 0;
}

// returns the final answer
double evaluate_postfix(Stack* eval_stack, Queue* outQueue) {
    double num;
    double ans;
    char temp_buff[64];
    int last_index = outQueue->tail-1;
    for (int i = 0; i < outQueue->tail; i++) {
        char* op = outQueue->head_pt[i];
        if ((num = atof(op)) != 0.0 || strcmp(op, "0") == 0) {
            push(eval_stack, op);
        } else {
            char *first, *last;
            last = pop(eval_stack);
            first = pop(eval_stack);
            ans = comp_expression(op, first, last);
            if (i != last_index) {
                snprintf(temp_buff, sizeof(temp_buff), "%.5f", ans);
                push(eval_stack, temp_buff);
            }
        }
    }

    return ans;
}

// 3421-*6*+8+
// 2 - 1:  1
// 4 * 1:  4
// 6 * 4:  24
// 3 + 24: 27
// 8 + 27: 35 
double comp_expression(char* op, char* first, char* last) {
    double a = atof(first); 
    double b = atof(last);

    if (strcmp(op, "+") == 0) {
        return a + b;
    } else if (strcmp(op, "-") == 0) {
        return a - b;
    } else if (strcmp(op, "*") == 0) {
        return a * b;
    } else if (strcmp(op, "/") == 0) {
        if (b == 0.0) {
            printf("Error: dividing by 0");
            exit(1); // TODO: handle this without crashing
        }
        return a / b;
    } else {
        printf("Error: invalid token");
        exit(1);
    }
}

// 3 + 4 * ( 2 - 1 ) * 6 + 8
// 3421-*6*+8+
// 35
int make_OutputQueue(Stack* op_stack, Queue* outQueue) 
{
    const int maxEquation = 1024;
    char* equation = malloc(sizeof(char)* maxEquation);
    
    int tokCount = 0;
    char* tok;

    printf(">> ");
    fgets(equation, maxEquation, stdin);
    equation[strlen(equation)-1] = '\0';
    if (equation == NULL) exit(1);

    for( tok = strtok_r(equation," ", &equation); tok!=NULL ; tok=strtok_r(NULL," ", &equation)) {
        tokCount++;
        float num;
        if ((num = atof(tok)) != 0.0 || strcmp(tok, "0") == 0) {
            enQueue(outQueue, tok);
        } else {
            int prec = getPrec(tok);
            switch(prec) 
            {
                // One of these: *, /, +, -
                case 1:
                case 2:
                    compare_operators(prec, op_stack, outQueue);
                    push(op_stack, tok);
                    break;
                case 3:
                    push(op_stack, tok);
                    break;
                case 4:
                    do {
                        enQueue(outQueue, pop(op_stack));
                    } while(strcmp(peek(op_stack), "(") != 0);
                    pop(op_stack); // get rid of open paren
                    break;
                case 0:
                    printf("Quitting Successfully");
                    free(equation);
                    return 0;
                case -1:
                    printf("error: invalid symbol '%s'", tok);
                    free(equation);
                    return -1;
            }
                 
        }
    }
    free(equation);

    if (!isEmpty(op_stack)) 
    {
        cleanStack(op_stack, outQueue);
    }
    
    return tokCount;
    
}

int getPrec(char* tok) 
{
    if (strcmp(tok, "+") == 0 || strcmp(tok, "-") == 0) {
        return 1; 
    } else if (strcmp(tok, "*") == 0 || strcmp(tok, "/") == 0) {
        return 2;
    } else if (strcmp(tok, "(") == 0) {
        return 3;
    } else if (strcmp(tok, ")") == 0) {
        return 4;
    }
    else if (strcmp(tok, "Q\n") == 0 || strcmp(tok, "q\n") == 0) {
        return 0;
    }
    else return -1;
}

void cleanStack(Stack* op_stack, Queue* outQueue) 
{
    for (int i = op_stack->count - 1; i > -1; i--) {
        enQueue(outQueue, op_stack->top[i]);
    }
}

void compare_operators(int prec, Stack* op_stack, Queue* outQueue) {
    if (!isEmpty(op_stack)) {
        int next_prec = getPrec(peek(op_stack));
    // enqueue the next thing if it's precedence is less than or same
        if (prec <= next_prec && next_prec != 3) {
            enQueue(outQueue, pop(op_stack));
            compare_operators(prec, op_stack, outQueue);
        }
    } return;
}
