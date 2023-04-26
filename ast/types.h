#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "utils.h"


typedef enum LiteralType {
    TYPE_IDENT,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_CHAR,
    TYPE_STRING,
    TYPE_BOOL
} LiteralType;


typedef struct IdentType {
    char* value;
} IndentType;

IndentType* ident_type_new(const char*);
void ident_type_to_string(IndentType**);
void ident_type_free(IndentType**);


typedef struct IntType {
    int value;
} IntType;

IntType* int_type_new(int);
void int_type_to_string(IntType**);
void int_type_free(IntType**);


typedef struct FloatType {
    double value;
} FloatType;

FloatType* float_type_new(double);
void float_type_to_string(FloatType**);
void float_type_free(FloatType**);


typedef struct CharType {
    char value;
} CharType;

CharType* char_type_new(char);
void char_type_to_string(CharType**);
void char_type_free(CharType**);


typedef struct StringType {
    char* value;
} StringType;

StringType* string_type_new(const char*);
void string_type_to_string(StringType**);
void string_type_free(StringType**);


typedef struct BoolType {
    bool value;
} BoolType;

BoolType* bool_type_new(bool);
void bool_type_to_string(BoolType**);
void bool_type_free(BoolType**);
