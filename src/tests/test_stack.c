#include <stdio.h> 
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "stack.h"

int main(void) {

    Stack stack = {
        .capacity = 5,
        .count = 0,
    };
    allocStack(&stack);

    for (int i = 0; i < 10; i ++) {
        Tok tok = {
            .id = INV,
            .value = (float)i,
            .prec = ARITHMETIC_FIRST,
        };
        push(&stack, tok);
    }

    printStack(&stack);
    return 0;

}