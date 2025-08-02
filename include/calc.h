
#ifndef CALC_H
#define CALC_H

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
    int tail;
    int head;
    int capacity;
    Tok* head_pt;
} Queue;

typedef struct 
{
    int count;
    int capacity;
    struct Tok* top;
} Stack;

bool eval_eq();
bool read_input(char* eq);
bool gen_and_push_num(char* operand_left, Stack* token_stack);
int generate_tokens(char* eq, Stack* token_stack);
void gen_and_push_op(char* op, Stack* token_stack);
void handle_paren(char* eq, int* op_index, int i, int* start_index, 
                  int* token_count, Stack* token_stack);
bool handle_arithmetic(char* eq, int* op_index, int i, int* start_index, int* token_count, 
                       char tok, Stack* token_stack, size_t eq_len);
bool handle_last_token(char* eq, size_t eq_len, int op_index, 
                       Stack* token_stack, int* token_count);
bool validate_parens(Stack* token_stack);
void make_output_queue(Stack* token_stack, Stack* op_stack, Queue* outqueue);
float solve(float a, float b, ID id);
float eval_queue(Queue* outqueue);


void allocStack(Stack* stack);
void push(Stack* stack, struct Tok op);
struct Tok pop(Stack* stack);
void printStack(Stack* stack);
Tok peek(Stack* stack);
PREC peek_at_prec(Stack* stack);
bool isEmpty(Stack* stack);

void allocQueue(Queue* queue);
void enQueue(Queue* queue, Tok tok);
void resizeQueue(Queue* queue);
Tok deQueue(Queue* queue);
void printQueue(Queue* queue);
#endif
// turn equation to postfix
// void reset_data_structs(Stack* op_stack, Queue* outqueue);
// int make_OutputQueue(Stack* op_stack, Queue* outQueue); // returns number of tokens or -1 if fails
// void handle_num_tok(char* c, Queue* outqueue);
// void handle_op_tok(char c, Stack* op_stack, Queue* outqueue);
// bool build_num(char* num, char c, int* num_index, int max_chars);
// void compare_to_stack(PREC prec, Stack* op_stack, Queue* outQueue);
// void handle_left_paren(Stack* op_stack, Queue* outQueue);
// void get_operator_id_and_prec(char c, Tok* tok);
// void cleanStack(Stack* op_stack, Queue* outQueue);

// evalueate expression
// double evaluate_postfix(Stack* eval_stack, Queue* outQueue);
// double comp_expression(char* op, char* first, char* last);
// void compare_operators(int prec, Stack* op_stack, Queue* outQueue);
