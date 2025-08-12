#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <assert.h>
#include <stddef.h>
#include "calc.h"

#define MAX_EQUATION 256
#define MAX_NUMBER 64
#define MAX_OP 3

void print_stack(Stack* stack) {
    char msg[256] = "";
    // fprintf(stderr, "DEBUG: printing stack\n");
    for (size_t i = 0; i < stack->count; i++) {
        Tok tok = stack->top[i];
        char* id_string = get_id_as_string(tok.id);
        if (strcmp(id_string, "NUM") != 0) {
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
    // fprintf(stderr, "DEBUG: printing queue\n");
    for (size_t i = 0; i < queue->tail; i++) {
        Tok tok = queue->head_pt[i];
        char* id_string = get_id_as_string(tok.id);
        if (strcmp(id_string, "NUM") != 0) {
            sprintf(&msg[strlen(msg)], "%s, ", id_string);
            // printf("MSG: %s", msg);
        } else {
            char num_string[50];
            sprintf(num_string, "%g", tok.value);
            sprintf(&msg[strlen(msg)], "%s, ", num_string);
        }
    }

    // fprintf(stderr, "%s\n", msg);
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
            return "NUM";
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
        case LEFT_PAREN:
        case RIGHT_PAREN:
        default: return -1;

    }
}

float eval_queue(Queue* outqueue) {
    Stack num_stack = {
        .capacity = 10,
        .count = 0,
    };
    allocStack(&num_stack);

    float answer;
    size_t count = outqueue->tail;
    // printf("DEBUG: Count is %d\n", count);
    for (size_t i = 0; i < count; i++) {
        Tok tok = dequeue(outqueue);
        // printf("DEBUG: Tok is %s\n", get_id_as_string(tok.id));
        if(tok.id == NUM) {
            push(&num_stack, tok);
        } else {
            float b = pop(&num_stack).value;
            float a = pop(&num_stack).value;
            // fprintf(stderr, "DEBUG: A:%f\tB:%f\n", a, b);
            float temp = solve(a, b, tok.id);
            if (outqueue->tail > 0) {
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

    for (size_t i = 0; i < token_stack->count; i++) {
        Tok tok = token_stack->top[i];
        switch(tok.id) {
            case NUM:
                enqueue(outqueue, tok);
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
                            enqueue(outqueue, pop(&op_stack));
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
                    } else enqueue(outqueue, pop(&op_stack));
                }
                break;
        }
        // print_queue(outqueue);
        // print_stack(&op_stack);
    }

    // if the stack is not empty enq everything
    if (!isEmpty(&op_stack)) {
        // printf("DEBUG: clean stack\n");
        size_t count = op_stack.count;
        for (size_t i = 0; i < count; i++) {
            enqueue(outqueue, pop(&op_stack));
            print_stack(&op_stack);
        }
    }

    free(op_stack.top);
}

bool validate_parens(Stack* token_stack) {
    for (size_t i = 0; i < token_stack->count; i++) {
        if (token_stack->top[i].id == LEFT_PAREN) {
            bool is_valid = false;
            for (size_t j = i+1; j < token_stack->count; j++) {
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

bool gen_and_push_num(char* num_buff, Stack* token_stack) {
    float val = atof(num_buff);
    if (val == 0.0f && strcmp(num_buff, "0") != 0) {
        fprintf(stderr, "ERROR: \"%s\" is not a valid number\n", num_buff);
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

// returns number of bystes offset or -1
size_t parse_num_tostring (char c, size_t i, char* num_buff, char* eq, size_t eq_len) {
    bool could_be_decimal = true;
    if (c == '.') {
        could_be_decimal = false;
    }

    size_t num_size = 1;
    if (i == eq_len) {
        strncpy(num_buff, eq+i, num_size);
        num_buff[num_size] = '\0';
        return num_size;
    }
    char next;
    for (size_t j = i+1; j < eq_len; j++) {
        next = eq[j]; 
        if (could_be_decimal) {
            if (next == '.') {
                if (next == '.') could_be_decimal = false;
                num_size++;
            }
        } else {
            if (isdigit(next) != 0) {
                num_size++;
            }
        }

        if (num_size > MAX_NUMBER) return -1;
        strncpy(num_buff, eq+i, num_size);
        num_buff[num_size] = '\0';
    } 
    return num_size;
}

size_t parse_op_tostring(char c, size_t i, char* op_buff, char* eq, size_t eq_len) {
    size_t op_size = 1;
    if (c == '*' && i+1 < eq_len && eq[i+1] == '*') op_size++;
    strncpy(op_buff, eq+i, op_size);
    op_buff[op_size] = '\0';
    return op_size;
}

// return the number of tokens
size_t tokenize_eq(char* eq, Stack* token_stack) {
    size_t eq_len = strlen(eq);
    char c;
    size_t token_count = 0;
    for (size_t i = 0; i < eq_len; i++) {
        c = eq[i];
        printf("c: %c\n", c);
        if (c == '+' || c == '*' || c == '/' || 
           (c == '-' && token_stack->count > 1 && 
            peek(token_stack).id == NUM) ||
            c == '(' || c == ')') {
                char op_buff[MAX_OP];
                size_t offset = parse_op_tostring(c, i, op_buff, eq, eq_len);
                gen_and_push_op(op_buff, token_stack);
                token_count++;
                printf("offset: %zu\n", offset);
                i += offset-1;

        } else if (isdigit(c) != 0 || c == '.' || c == '-') {
            char num_buff[MAX_NUMBER];
            size_t offset = parse_num_tostring(c, i, num_buff, eq, eq_len);
            if (!gen_and_push_num(num_buff, token_stack)) return -1;
            else token_count++;
            printf("offset: %zu\n", offset);
            i += offset-1;
        }
    }
    
    return token_count;
}

bool read_input(char* eq) {
    printf("Max Equation size is %d bytes\n", MAX_EQUATION);
    printf("Max Number size is %d bytes\n", MAX_NUMBER);
    printf(">> ");
    size_t i = 0;
    char c;
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
    // fprintf(stderr, "DEBUG: Equation: %s\n", eq);
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
    tokenize_eq(eq, &token_stack); 
    print_stack(&token_stack);
    exit(1);
    if (tokenize_eq(eq, &token_stack) < 3) {
        fprintf(stderr, "Error: not a valid equation\n");
        return false;
    } 

    // print_stack(&token_stack);

    if (!validate_parens(&token_stack)) return false;

    Queue outqueue = {
        .capacity = 10,
        .tail = 0,
    };
    allocQueue(&outqueue);

    make_output_queue(&token_stack, &outqueue);
    print_stack(&token_stack);
    free(token_stack.top);
    print_queue(&outqueue);
    // exit(1);

    float answer = eval_queue(&outqueue);
    printf("%g\n", answer);
    free(outqueue.head_pt);
    return true;
}

int main(void) {
    bool should_quit = false;
    do {
        char msg[256];
        int c;
        size_t count = 0;
        printf(" ----------- cli-(C)alc -----------\n"
               "     1. Evaluate Equation\n"
               "     2. Quit\n");
         while ((c = getchar()) != '\n')  {
            msg[count] = c;
            count++;
        }
        msg[count] = '\0';
        // printf("c:%s\n", msg);
        // printf("%zu\n", strlen(msg));
        if (strlen(msg) > 1) {
            fprintf(stderr, "ERROR: Invalid option, type 1 or 2\n");
        } else if (strcmp(msg, "1") == 0) {
            should_quit = eval_eq();
        } else if (strcmp(msg, "2") == 0) {
            should_quit = true;
        } else {
            fprintf(stderr, "ERROR: invalid option\n");
        }
    } while (!should_quit);
    
    return 0;
}