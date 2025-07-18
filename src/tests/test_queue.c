#include <stdio.h> 
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "queue.h"
#include "calc.h"

int main(void) {
     
    Queue queue = {
        .capacity = 5,
        .tail = 0,
        .head = 0,
    };
    allocQueue(&queue);

    for (int i  = 0; i < 10; i++) {
        Tok tok = {
            .id = INV,
            .value = (float)i,
            .prec = ARITHMETIC_FIRST,
        };

        enQueue(&queue, tok);
    }

    printQueue(&queue);

    return 0;
}