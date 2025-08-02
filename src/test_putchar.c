#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "calc.h"
// #include "stack.h"
// #include "queue.h"

#define MAX_EQUATION 256
#define MAX_NUMBER 64
#define MAX_OP 3

int main(void) {
    bool should_quit = false;
    char c;
    do {
        printf(" ----------- C-alc -----------\n"
               "     1. Evaluate Equation\n"
               "     2. Quit\n");
        c = getchar();
        if (c == '2') should_quit = true;
        else if (c == '1') {
            while ((c = getchar()) != '\n' && c != EOF); // clear input buffer
            if (eval_eq()) should_quit = true;
        }
    } while (!should_quit);
    
    return 0;
}

bool read_input(char* eq) {
    printf("Max Equation size is %d bytes\n", MAX_EQUATION);
    printf("Max Number size is %d bytes\n", MAX_NUMBER);
    printf(">> ");
    int i = 0;
    int c;
    while((c = getchar()) != '\n') {
        if (i == MAX_EQUATION-1) {
            printf("ERROR: Your equation is too big\n");
            return false;
        }
        else if (c != ' ' && c != '\t'){
            eq[i] = c;
            i++;
        }
    }
    eq[i] = '\0';
    printf("Equation: %s\n", eq);
    return true;
    
}

bool eval_eq() {
    char eq[MAX_EQUATION];
    if (!read_input(eq)) return false;
    
    Stack token_stack = {
        .capacity = 10,
        .count = 0,
    };
    allocStack(&token_stack);

    if (generate_tokens(eq, &token_stack) < 3) {
        printf("Error: not a valid equation\n");
        return false;
    }
    printStack(&token_stack);

    if (!validate_parens(&token_stack)) return false;

    Queue outqueue = {
        .capacity = 10,
        .tail = 0,
    };
    allocQueue(&outqueue);

    Stack op_stack = {
        .capacity = 10,
        .count = 0,
    };
    allocStack(&op_stack);

    make_output_queue(&token_stack, &op_stack, &outqueue);
    free(token_stack.top);
    printQueue(&outqueue);

    float answer = eval_queue(&outqueue);
    printf("%.5f\n", answer);
    return true;
}

float solve(float a, float b, ID id) {
    printf("reached here\n");
    switch(id) {
        case ADD:
            printf("reached ADD\n");
            printf("answ: %f\n", a + b);
            return a + b;
        case SUB:
            return a - b;
        case MUL:
            return a * b;
        case DIV:
            return a / b;
        case EXPR:
            return pow(a, b);

    }
    return -1.0f;
}

float eval_queue(Queue* outqueue) {
    Stack num_stack = {
        .capacity = 10,
        .count = 0,
    };
    allocStack(&num_stack);

    float answer;
    for (int i = 0; i < outqueue->tail; i++) {
        Tok tok = outqueue->head_pt[i];
        if(tok.id == NUM) {
            push(&num_stack, tok);
        } else {
            float a = pop(&num_stack).value;
            float b = pop(&num_stack).value;
            printf("A:%f\tB:%f\n", a, b);
            float temp = solve(a, b, tok.id);
            if (num_stack.count > 0) {
                push(&num_stack, (Tok) {-1, temp, -1});
            } else {
                answer = temp;
                break;
            }
        }
    }
    free(num_stack.top);
    return answer;
}
void make_output_queue(Stack* token_stack, Stack* op_stack, Queue* outqueue) {
    printf("REACHED: make output\n");
    for (int i = 0; i < token_stack->count; i++) {
        Tok tok = token_stack->top[i];
        switch(tok.id) {
            case NUM:
                enQueue(outqueue, tok);
                break;
            case ADD:
            case SUB:
            case DIV:
            case MUL:
            case EXPR:
                if (isEmpty(op_stack)) {
                    push(op_stack, tok);
                } else {
                    do {
                        PREC next_prec = peek(op_stack).prec;
                        if (next_prec > tok.prec && next_prec != PAREN) {
                            enQueue(outqueue, pop(op_stack));
                        } else break;
                    } while(!isEmpty(op_stack));

                    push(op_stack, tok);
                }
                break;
            case LEFT_PAREN:
                push(op_stack, tok);
                break;
            case RIGHT_PAREN:
                while (!isEmpty(op_stack)) {
                    Tok next_tok = pop(op_stack);
                    if (next_tok.id == RIGHT_PAREN) break;
                    else enQueue(outqueue, pop(op_stack));
                }
                pop(op_stack);
                break;
        }
    }

    if (!isEmpty(op_stack)) {
        for (int i = 0; i < op_stack->count; i++) {
            enQueue(outqueue, pop(op_stack));
        }
    }

    free(op_stack->top);
}

bool validate_parens(Stack* token_stack) {
    printf("REACHED: validate parens\n");
    bool is_valid = true;
    for (int i = 0; (i < token_stack->count && is_valid); i++) {
        if (token_stack->top[i].id == LEFT_PAREN) {
            for (int j = i+1; j < token_stack->count; j++) {
                if (token_stack->top[j].id == RIGHT_PAREN) {
                    break;
                }
            }
            is_valid = false; 
            break;
        } printf("NOT A LEFT PAREN\n");
    }
    printf("IS_VALID: %d\n", is_valid);
    return is_valid;
}
// return the number of tokens
int generate_tokens(char* eq, Stack* token_stack) {

    char tok;
    int start_index = 0;
    int eq_len = strlen(eq);
    int op_index;
    int token_count = 0;
    for (int i = 0; i < eq_len; i++) {
        tok = eq[i];
        printf("TOK: %c\n", tok);
        printf("token_count: %d\n", token_count);
        if ((tok == '-' && i != start_index)|| tok == '+' || tok == '*') 
        {
            printf("OP at index %d\n", i);
            if (!handle_arithmetic(eq, &op_index, i, &start_index, 
                                   &token_count, tok, 
                                   token_stack, eq_len)) 
                return -1;
        }
        else if ((tok == '(' && i < eq_len-1)|| tok == ')') {
            printf("OP at index %d\n", i);
            handle_paren(eq, &op_index, i, &start_index, 
                         &token_count, token_stack);
        }

    }

    if (op_index < eq_len-1) {
        handle_last_token(eq, eq_len, op_index, 
                          token_stack, &token_count);
    }
    
    return token_count;
}

// 1 + (2*2)
void handle_paren(char* eq, int* op_index, int i, int* start_index, 
                  int* token_count, Stack* token_stack) 
{
    *op_index = i;
    char paren_buf[MAX_OP];
    strncpy(paren_buf, eq+i, 1);
    paren_buf[1] = '\0';
    printf("PAREN: %s\n", paren_buf);
    gen_and_push_op(paren_buf, token_stack);
    *start_index = i + 1;
    (*token_count)++;
}

bool handle_arithmetic(char* eq, int* op_index, int i, int* start_index, int* token_count, 
                       char tok, Stack* token_stack, size_t eq_len) 
{
    *op_index = i;
    size_t len = i - *start_index;
    if (len > MAX_NUMBER - 1) {
        printf("ERROR: token number %d was %zu bytes too big\n", (*token_count)+1, len);
        return false;
    } else {
        char operand_left[MAX_NUMBER];
        printf("I: %d, LEN: %zu\n", i, len);
        strncpy(operand_left, eq+(i-len), len);
        operand_left[len] = '\0';
        printf("op_left: %s\n", operand_left);
        if (!gen_and_push_num(operand_left, token_stack)) return false;
    }

    char op_buf[MAX_OP];
    size_t count;
    if (tok == '*' && (eq[i+1] == '*' && i < (int)eq_len)) {
        count = 2;
    } else {
        count = 1;
    }
    strncpy(op_buf, eq+i, count);
    op_buf[count] = '\0';

    gen_and_push_op(op_buf, token_stack);
    *start_index = i+count;
    (*token_count)+=2;

    return true;
}

bool handle_last_token(char* eq, size_t eq_len, int op_index, 
                       Stack* token_stack, int* token_count) 
{
    char end_operand[MAX_NUMBER];
    printf("EQ_LEN: %zu OP_INDEX: %d", eq_len, op_index);
    size_t count = eq_len-1-op_index;
    if (count > MAX_NUMBER) return false;
    strncpy(end_operand, eq+op_index+1, count);
    end_operand[count] = '\0';
    if (!gen_and_push_num(end_operand, token_stack)) return false;
    (*token_count)++;
    return true;
}

void gen_and_push_op(char* op, Stack* token_stack) {
    Tok op_token = {
        .value = -1,
    };

    if (strcmp(op, "+") == 0) {
        op_token.id = ADD;
        op_token.prec = PRIMARY;
    } if (strcmp(op, "-") == 0) {
        op_token.id = SUB;
        op_token.prec = PRIMARY;
    } if (strcmp(op, "*") == 0) {
        op_token.id = MUL;
        op_token.prec = SECONDARY;
    } if (strcmp(op, "/") == 0) {
        op_token.id = DIV;
        op_token.prec = SECONDARY;
    } if (strcmp(op, "**") == 0) {
        op_token.id = EXPR;
        op_token.prec = TERTIARY;
    } if (strcmp(op, "(") == 0) {
        op_token.id = LEFT_PAREN;
        op_token.prec = PAREN;
    } if (strcmp(op, ")") == 0) {
        op_token.id = RIGHT_PAREN;
        op_token.prec = PAREN;
    }

    push(token_stack, op_token);
}

bool gen_and_push_num(char* operand_left, Stack* token_stack) {
    float val = atof(operand_left);
    printf("NUM STRING: %s\n", operand_left);
    printf("NUM VALUE: %f\n", val);
    if (val == 0.0f && strcmp(operand_left, "0") != 0) {
        printf("ERROR: It's not a valid number \n");
        return false;
    } else {
        Tok tok = {
            .value = val,
            .id = NUM,
            .prec = -1,
        };
        push(token_stack, tok);
        return true;
    }
}