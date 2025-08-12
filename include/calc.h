
#ifndef CALC_H
#define CALC_H

#include <stdbool.h>
#include <stddef.h>
#include <ctype.h>

typedef enum ID{
    NUM,
    ADD,
    SUB,
    MUL,
    DIV,
    EXPR,
    LEFT_PAREN,
    RIGHT_PAREN,
}ID;

typedef enum PREC {
    PRIMARY,
    SECONDARY,
    TERTIARY,
    PAREN,
}PREC;

typedef struct Tok {
    enum ID id;
    float value;
    PREC prec;
} Tok;

typedef struct Queue
{
    size_t tail;
    size_t head;
    size_t capacity;
    Tok* head_pt;
} Queue;

typedef struct 
{
    size_t count;
    size_t capacity;
    struct Tok* top;
} Stack;

// calc.c
bool eval_eq();
bool read_input(char* eq);
bool gen_and_push_num(char* operand_left, Stack* token_stack);
size_t tokenize_eq(char* eq, Stack* token_stack);
void gen_and_push_op(char* op, Stack* token_stack);
size_t parse_op_tostring(char c, size_t i, char* op_buff, char* eq, size_t eq_len);
size_t parse_num_tostring (char c, size_t i, char* num_buff, char* eq, size_t eq_len);
bool validate_parens(Stack* token_stack);
void make_output_queue(Stack* token_stack, Queue* outqueue);
float solve(float a, float b, ID id);
float eval_queue(Queue* outqueue);
char* get_id_as_string(ID id);
void print_stack(Stack* stack);
void print_queue(Queue* queue);

// stack.c
void allocStack(Stack* stack);
void push(Stack* stack, struct Tok op);
struct Tok pop(Stack* stack);
void printStack(Stack* stack);
Tok peek(Stack* stack);
PREC peek_at_prec(Stack* stack);
bool isEmpty(Stack* stack);

// queue.c
void allocQueue(Queue* queue);
void enqueue(Queue* queue, Tok tok);
void resizeQueue(Queue* queue);
Tok dequeue(Queue* queue);
void printQueue(Queue* queue);
#endif

