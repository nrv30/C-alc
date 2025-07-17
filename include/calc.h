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
};

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

typedef enum PREC {
    ARITHMETIC_FIRST,
    ARITHMETIC_SECONDARY,
    PAREN,
};

typedef struct Tok {
    enum ID id;
    float value;
    enum PREC prec;
} Tok;
