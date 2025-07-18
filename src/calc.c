#include <stdio.h>
#include <stdbool.h> 
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "stack.h"
#include "queue.h"
#include "calc.h"

// turn equation to postfix
void reset_data_structs(Stack* op_stack, Queue* outqueue);
int make_OutputQueue(Stack* op_stack, Queue* outQueue); // returns number of tokens or -1 if fails
void handle_num_tok(char* c, Queue* outqueue);
void handle_op_tok(char c, Stack* op_stack, Queue* outqueue);
bool build_num(char* num, char c, int* num_index, int max_chars);
void compare_to_stack(PREC prec, Stack* op_stack, Queue* outQueue);
void handle_left_paren(Stack* op_stack, Queue* outQueue);
void get_operator_id_and_prec(char c, Tok* tok);
void cleanStack(Stack* op_stack, Queue* outQueue);

// evalueate expression
// double evaluate_postfix(Stack* eval_stack, Queue* outQueue);
// double comp_expression(char* op, char* first, char* last);
// void compare_operators(int prec, Stack* op_stack, Queue* outQueue);

char ID_table[8][64] = {
    "ADD",
    "SUB",
    "MULT",
    "DIV",
    "NUM",
    "LEFT_PAREN",
    "RIGHT_PAREN",
    "INV",
};

int main(void) 
{
    Stack op_stack = {
        .capacity = 5,
    };
    allocStack(&op_stack);

    Queue outqueue = {
        .capacity = 5,
    };
    allocQueue(&outqueue);

    const int minlen = 3; // smallest equation
    int tok_count = 0;
    do {
        reset_data_structs(&op_stack, &outqueue);
        tok_count = make_OutputQueue(&op_stack, &outqueue);
    } while(tok_count < minlen);
    
    free(op_stack.top);
    printQueue(&outqueue);
    free(outqueue.head_pt);
    
    // Stack eval_stack;
    // eval_stack.count = -1;
    // eval_stack.capacity = 5;
    // allocStack(&eval_stack);
    
    // double ans = evaluate_postfix(&eval_stack, &outQueue);
    
    // freeQueue(&outQueue);
    // freeStack(&eval_stack);
    
    // printf("%f\n", ans);
    // printf(">>\n");
    
    return 0;
}

// returns the final answer
// double evaluate_postfix(Stack* eval_stack, Queue* outQueue) {
//     double num;
//     double ans;
//     char temp_buff[64];
//     int last_index = outQueue->tail-1;
//     for (int i = 0; i < outQueue->tail; i++) {
//         char* op = outQueue->head_pt[i];
//         if ((num = atof(op)) != 0.0 || strcmp(op, "0") == 0) {
//             push(eval_stack, op);
//         } else {
//             char *first, *last;
//             last = pop(eval_stack);
//             first = pop(eval_stack);
//             ans = comp_expression(op, first, last);
//             if (i != last_index) {
//                 snprintf(temp_buff, sizeof(temp_buff), "%.5f", ans);
//                 push(eval_stack, temp_buff);
//             }
//         }
//     }

//     return ans;
// }

// 3421-*6*+8+
// 2 - 1:  1
// 4 * 1:  4
// 6 * 4:  24
// 3 + 24: 27
// 8 + 27: 35 
// double comp_expression(char* op, char* first, char* last) {
//     double a = atof(first); 
//     double b = atof(last);

//     if (strcmp(op, "+") == 0) {
//         return a + b;
//     } else if (strcmp(op, "-") == 0) {
//         return a - b;
//     } else if (strcmp(op, "*") == 0) {
//         return a * b;
//     } else if (strcmp(op, "/") == 0) {
//         if (b == 0.0) {
//             printf("Error: dividing by 0");
//             exit(1); // TODO: handle this without crashing
//         }
//         return a / b;
//     } else {
//         printf("Error: invalid token");
//         exit(1);
//     }
// }

// 3 + 4 * ( 2 - 1 ) * 6 + 8
// 3421-*6*+8+
// 35
void reset_data_structs(Stack* op_stack, Queue* outqueue) {
    op_stack->count = 0;
    outqueue->head = 0;
    outqueue->tail = 0;
}

int make_OutputQueue(Stack* op_stack, Queue* outqueue) 
{
    const int max_eq_size = 50;
    char equation[max_eq_size];

    printf("<< ");   
    fgets(equation, max_eq_size, stdin);
    equation[strlen(equation-1)] = '\0';
    int eqlen = (int)strlen(equation);

    char back_c;
    int last_op_index = eqlen-1;
    while (back_c != '-' || back_c != '+' || 
           back_c != "*" || back_c != "/") 
    {
        back_c = equation[--last_op_index];
    }

    int token_count = 0;
    const int max_chars = 50;
    char num[max_chars]; 
    int num_index = 0;
    char c;
    for (int i = 0; i < last_op_index; i++) {
        c = equation[i];
        if (c == '-') {
            if (!build_num(num, c, &num_index, max_chars)) {
                handle_op_tok(c, op_stack, outqueue); // the minus sign was an operator
            }
        } else if (isdigit(c) || c == '.') {
            if (!build_num(num, c, &num_index, max_chars)) {
                    printf("ERROR: number exceeded max characters %d\n", max_chars);
                    return 1;
                }
        } else if (c == '+' || c == '/' || c == '*' || c == '-') { // encountered an operator not minus
            handle_num_tok(num, outqueue); 
            handle_op_tok(c, op_stack, outqueue);
            // rewrite index to overwrite the number buffer
            token_count+=2;
            num_index = 0;
        } else if (c == ' ') {
            printf("ERROR: Please omit spaces between tokens\n");
            return 1;
        } else {
            printf("ERROR: Invalid character %c\n", c);
            return 1;
        }
    }
    
    for(int i = ++last_op_index; i < eqlen; i++) {
        num[num_index] = i;
        num_index++;
    }
    handle_num_tok()
    cleanStack(op_stack, outqueue);
    return token_count;
}

void handle_num_tok(char* c, Queue* outqueue) {
    Tok tok = {
        .id = NUM,
        .value = atof(c),
        .prec = -1,
    };
    enQueue(outqueue, tok);
}

void handle_op_tok(char c, Stack* op_stack, Queue* outqueue) {
    Tok tok;
    tok.value = -1;
    get_operator_id_and_prec(c, &tok);
    if (tok.id == RIGHT_PAREN) {
        push(op_stack, tok);
    } else if (tok.id == LEFT_PAREN) {
        handle_left_paren(op_stack, outqueue);
    } else {
        if (!isEmpty(op_stack)) {
            compare_to_stack(tok.prec, op_stack, outqueue);
        } else {
            push(op_stack, tok);
        }
    }
}

bool build_num(char* num, char c, int* num_index, int max_chars) {
    if (*num_index > max_chars) return false;
    else {
        return true;
        num[*num_index] = c;
        *num_index++;
    }
}

void compare_to_stack(PREC prec, Stack* op_stack, Queue* outQueue) {
    PREC next_prec = peek_at_prec(op_stack);
    // enqueue the next thing if it's precedence is less than or same
    if (prec <= next_prec && next_prec != 3) {
        enQueue(outQueue, pop(op_stack));
        compare_to_stack(prec, op_stack, outQueue);
    }
}

void handle_left_paren(Stack* op_stack, Queue* outQueue) {
    // TODO: handle case there is no matching right 
    while (!isEmpty(op_stack), peek(op_stack).id != RIGHT_PAREN) {
        enQueue(outQueue, pop(op_stack));
    }
    pop(op_stack);
}

// fill the prec and id fields for the operator token
void get_operator_id_and_prec(char c, Tok* tok) {
    ID id;
    PREC prec;
    switch (c) {
        case '+':
            id = ADD;
            prec = ARITHMETIC_FIRST;
            break;
        case '-':
            id = SUB;
            prec = ARITHMETIC_FIRST;
            break;
        case '*':
            id  = MULT;
            prec = ARITHMETIC_SECONDARY;
            break;
        case '(':
            id = LEFT_PAREN;
            prec = PAREN;
            break;
        case ')':
            id = RIGHT_PAREN;
            prec = PAREN;
            break;
        default:
            break;
    }

    tok->id = id;
    tok->prec = prec;
}

void cleanStack(Stack* op_stack, Queue* outQueue) 
{
    for (int i = op_stack->count - 1; i > -1; i--) {
        enQueue(outQueue, op_stack->top[i]);
    }
}


