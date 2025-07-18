#pragma once

typedef enum ID{
    ADD,
    SUB,
    MULT,
    DIV,
    NUM,
    LEFT_PAREN,
    RIGHT_PAREN,
    INV,
}ID;
extern char ID_table[8][64];

typedef enum PREC {
    ARITHMETIC_FIRST,
    ARITHMETIC_SECONDARY,
    PAREN,
}PREC;

typedef struct Tok {
    enum ID id;
    float value;
    PREC prec;
} Tok;
