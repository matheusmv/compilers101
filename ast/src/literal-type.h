#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "utils.h"


typedef enum LiteralType {
    IDENT_LITERAL,
    INT_LITERAL,
    FLOAT_LITERAL,
    CHAR_LITERAL,
    STRING_LITERAL,
    BOOL_LITERAL,
    VOID_LITERAL,
    NIL_LITERAL
} LiteralType;


typedef struct IdentLiteral {
    char* value;
} IndentLiteral;

IndentLiteral* ident_literal_new(const char*);
void ident_literal_to_string(IndentLiteral**);
void ident_literal_free(IndentLiteral**);


typedef struct IntLiteral {
    int value;
} IntLiteral;

IntLiteral* int_literal_new(int);
void int_literal_to_string(IntLiteral**);
void int_literal_free(IntLiteral**);


typedef struct FloatLiteral {
    double value;
} FloatLiteral;

FloatLiteral* float_literal_new(double);
void float_literal_to_string(FloatLiteral**);
void float_literal_free(FloatLiteral**);


typedef struct CharLiteral {
    char value;
} CharLiteral;

CharLiteral* char_literal_new(char);
void char_literal_to_string(CharLiteral**);
void char_literal_free(CharLiteral**);


typedef struct StringLiteral {
    char* value;
} StringLiteral;

StringLiteral* string_literal_new(const char*);
void string_literal_to_string(StringLiteral**);
void string_literal_free(StringLiteral**);


typedef struct BoolLiteral {
    bool value;
} BoolLiteral;

BoolLiteral* bool_literal_new(bool);
void bool_literal_to_string(BoolLiteral**);
void bool_literal_free(BoolLiteral**);

typedef struct VoidLiteral {
    void* _;
} VoidLiteral;

VoidLiteral* void_literal_new(void);
void void_literal_to_string(VoidLiteral**);
void void_literal_free(VoidLiteral**);

typedef struct NilLiteral {
    void* _;
} NilLiteral;

NilLiteral* nil_literal_new(void);
void nil_literal_to_string(NilLiteral**);
void nil_literal_free(NilLiteral**);
