#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <assert.h>
#include "calc.h"

#define MAX_EQUATION 256
#define MAX_NUMBER 64
#define MAX_OP 3

void print_stack(Stack* stack) {
    char msg[256] = "";
    fprintf(stderr, "DEBUG: printing stack\n");
    for (int i = 0; i < stack->count; i++) {
        Tok tok = stack->top[i];
        char* id_string = get_id_as_string(tok.id);
        if (strcmp(id_string, "") != 0) {
            sprintf(&msg[strlen(msg)], "%s, ", id_string);
            // printf("MSG: %s", msg);
        } else {
            char num_string[50];
            sprintf(num_string, "%g", tok.value);
            sprintf(&msg[strlen(msg)], "%s, ", num_string);
        }
    }

    fprintf(stderr, "%s\n", msg);
}

void print_queue(Queue* queue) {
    char msg[256] = "";
    fprintf(stderr, "DEBUG: printing queue\n");
    for (int i = 0; i < queue->tail; i++) {
        Tok tok = queue->head_pt[i];
        char* id_string = get_id_as_string(tok.id);
        if (strcmp(id_string, "") != 0) {
            sprintf(&msg[strlen(msg)], "%s, ", id_string);
            // printf("MSG: %s", msg);
        } else {
            char num_string[50];
            sprintf(num_string, "%g", tok.value);
            sprintf(&msg[strlen(msg)], "%s, ", num_string);
        }
    }

    fprintf(stderr, "%s\n", msg);
}

char* get_id_as_string(ID id) {
    switch(id) {
        case ADD:
            return "+";
        case SUB:
            return "-";
        case MUL:
            return "*";
        case DIV:
            return "/";
        case LEFT_PAREN:
            return "(";
        case RIGHT_PAREN:
            return ")";
        case EXPR:
            return "**";
        case NUM:
            return "";
        default:
            return ""; 
    }
}

float solve(float a, float b, ID id) {
    switch(id) {
        case ADD:
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
            fprintf(stderr, "DEBUG: A:%f\tB:%f\n", a, b);
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

// 1 + (2*2)
void make_output_queue(Stack* token_stack, Queue* outqueue) {
    Stack op_stack = {
        .capacity = 10,
        .count = 0,
    };
    allocStack(&op_stack);

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
                if (isEmpty(&op_stack)) {
                    push(&op_stack, tok);
                } else {
                    do {
                        PREC next_prec = peek(&op_stack).prec;
                        if (next_prec >= tok.prec && next_prec != PAREN) {
                            enQueue(outqueue, pop(&op_stack));
                        } else break;
                    } while(!isEmpty(&op_stack));

                    push(&op_stack, tok);
                }
                break;
            case LEFT_PAREN:
                push(&op_stack, tok);
                break;
            case RIGHT_PAREN:
                while (!isEmpty(&op_stack)) {
                    ID next_id = peek(&op_stack).id;
                    if (next_id == LEFT_PAREN) {
                        pop(&op_stack);
                        break;
                    } else enQueue(outqueue, pop(&op_stack));
                }
                break;
        }
        print_queue(outqueue);
        print_stack(&op_stack);
    }

    // if the stack is not empty enq everything
    if (!isEmpty(&op_stack)) {
        printf("DEBUG: clean stack\n");
        int count = op_stack.count;
        for (int i = 0; i < count; i++) {
            enQueue(outqueue, pop(&op_stack));
            print_stack(&op_stack);
        }
    }

    free(op_stack.top);
}

bool validate_parens(Stack* token_stack) {
    for (int i = 0; i < token_stack->count; i++) {
        if (token_stack->top[i].id == LEFT_PAREN) {
            bool is_valid = false;
            for (int j = i+1; j < token_stack->count; j++) {
                if (token_stack->top[j].id == RIGHT_PAREN) {
                    is_valid = true;
                    break;
                }
            }
            if (!is_valid) return false;    
        }
    }
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
    if (val == 0.0f && strcmp(operand_left, "0") != 0) {
        fprintf(stderr, "ERROR: \"%s\" is not a valid number\n", operand_left);
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

void handle_paren(char* eq, int* op_index, int i, int* start_index, 
                  int* token_count, Stack* token_stack, char tok) 
{
    if (tok == ')') {
        char operand_left[MAX_NUMBER];
        size_t count = i - (*start_index);
        strncpy(operand_left, eq+(*start_index), count);
        operand_left[count] = '\0';
        gen_and_push_num(operand_left, token_stack);
        // printf("i: %d, op_index: %d, start_index %d\n", i, *op_index, *start_index); exit(1);
    }
    *op_index = i;
    char paren_buf[MAX_OP];
    strncpy(paren_buf, eq+i, 1);
    paren_buf[1] = '\0';
    // printf("PAREN: %s\n", paren_buf);
    gen_and_push_op(paren_buf, token_stack);
    *start_index = i + 1;
    (*token_count)++;
}

bool handle_arithmetic(char* eq, int* op_index, int i, int* start_index, int* token_count, 
                       char tok, Stack* token_stack, size_t eq_len) 
{
    if (i != *start_index) {
        *op_index = i;
        // printf("DEBUG: start_index: %d\n", *start_index);
        // printf("DEBUG: i is %d\n", i);
        size_t len = i - *start_index;
        if (len > MAX_NUMBER - 1) {
            fprintf(stderr, "ERROR: token number %d was %zu bytes too big\n", (*token_count)+1, len);
            return false;
        } else {
            char operand_left[MAX_NUMBER];
            // printf("I: %d, LEN: %zu\n", i, len);
            strncpy(operand_left, eq+(i-len), len);
            operand_left[len] = '\0';
            // printf("op_left: %s\n", operand_left);
            if (!gen_and_push_num(operand_left, token_stack)) return false;
        }
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
    // printf("EQ_LEN: %zu OP_INDEX: %d", eq_len, op_index);
    size_t count = eq_len-1-op_index;
    if (count > MAX_NUMBER) return false;
    strncpy(end_operand, eq+op_index+1, count);
    end_operand[count] = '\0';
    if (!gen_and_push_num(end_operand, token_stack)) return false;
    (*token_count)++;
    return true;
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
        // printf("token_count: %d\n", token_count);
        bool minus_is_op = true;
        if (tok == '-' && start_index == i && token_count > 0) {
            if (peek(token_stack).id != RIGHT_PAREN) {
                minus_is_op = false;
            }
        }
        if (((minus_is_op && tok == '-')|| tok == '+' || tok == '*')) 
        {
            printf("OP at index %d\n", i);
            if (!handle_arithmetic(eq, &op_index, i, &start_index, 
                                   &token_count, tok, 
                                   token_stack, eq_len)) 
                return -1;
        }
        else if ((tok == '(' && i < eq_len-1) || (tok == ')' && token_count > 0)) {
            handle_paren(eq, &op_index, i, &start_index, 
                         &token_count, token_stack, tok);
        }
    }

    handle_last_token(eq, eq_len, op_index, token_stack, &token_count);
    
    return token_count;
}

bool read_input(char* eq) {
    printf("Max Equation size is %d bytes\n", MAX_EQUATION);
    printf("Max Number size is %d bytes\n", MAX_NUMBER);
    printf(">> ");
    int i = 0;
    int c;
    while((c = getchar()) != '\n') {
        if (i == MAX_EQUATION-1) {
            fprintf(stderr, "ERROR: Your equation is too big\n");
            return false;
        }
        else if (c != ' ' && c != '\t'){
            eq[i] = c;
            i++;
        }
    }
    eq[i] = '\0';
    fprintf(stderr, "DEBUG: Equation: %s\n", eq);
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
        // fprintf(stderr, "Error: not a valid equation\n");
        return false;
    } 

    print_stack(&token_stack);

    if (!validate_parens(&token_stack)) return false;

    Queue outqueue = {
        .capacity = 10,
        .tail = 0,
    };
    allocQueue(&outqueue);

    make_output_queue(&token_stack, &outqueue);
    free(token_stack.top);
    print_queue(&outqueue);
    // exit(1);

    float answer = eval_queue(&outqueue);
    printf("%g\n", answer);
    return true;
}

int main(void) {
    bool should_quit = false;
    char c, temp;
    do {
        printf(" ----------- C-alc -----------\n"
               "     1. Evaluate Equation\n"
               "     2. Quit\n");
        c = getchar();
        while ((temp = getchar()) != '\n' && temp != EOF); // clear input buffer
        if (c == '1') {
            should_quit = eval_eq();
        } else if (c == '2') {
            should_quit = true;
        } else {
            fprintf(stderr, "ERROR: invalid character\n");
        }
    } while (!should_quit);
    
    return 0;
}