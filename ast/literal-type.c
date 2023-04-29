#include "literal-type.h"

#include <stdio.h>
#include <stdlib.h>


IndentLiteral* ident_literal_new(const char* ident) {
    IndentLiteral* type = NULL;
    type = malloc(sizeof(IndentLiteral));
    if (type == NULL) {
        return NULL;
    }

    type->value = str_dup(ident);

    return type;
}

void ident_literal_to_string(IndentLiteral** identType) {
    if (identType == NULL || *identType == NULL)
        return;

    printf("&[%s]", (*identType)->value);
}

void ident_literal_free(IndentLiteral** identType) {
    if (identType == NULL || *identType == NULL)
        return;

    free((*identType)->value);

    free(*identType);
    *identType = NULL;
}

IntLiteral* int_literal_new(int value) {
    IntLiteral* type = NULL;
    type = malloc(sizeof(IntLiteral));
    if (type == NULL) {
        return NULL;
    }

    type->value = value;

    return type;
}

void int_literal_to_string(IntLiteral** intLiteral) {
    if (intLiteral == NULL || *intLiteral == NULL)
        return;

    printf("%d", (*intLiteral)->value);
}

void int_literal_free(IntLiteral** intLiteral) {
    if (intLiteral == NULL || *intLiteral == NULL)
        return;

    free(*intLiteral);
    *intLiteral = NULL;
}

FloatLiteral* float_literal_new(double value) {
    FloatLiteral* type = NULL;
    type = malloc(sizeof(FloatLiteral));
    if (type == NULL) {
        return NULL;
    }

    type->value = value;

    return type;
}

void float_literal_to_string(FloatLiteral** floatLiteral) {
    if (floatLiteral == NULL || *floatLiteral == NULL)
        return;

    printf("%f", (*floatLiteral)->value);
}

void float_literal_free(FloatLiteral** floatLiteral) {
    if (floatLiteral == NULL || *floatLiteral == NULL)
        return;

    free(*floatLiteral);
    *floatLiteral = NULL;
}

CharLiteral* char_literal_new(char value) {
    CharLiteral* type = NULL;
    type = malloc(sizeof(CharLiteral));
    if (type == NULL) {
        return NULL;
    }

    type->value = value;

    return type;
}

void char_literal_to_string(CharLiteral** charLiteral) {
    if (charLiteral == NULL || *charLiteral == NULL)
        return;

    printf("'%c'", (*charLiteral)->value);
}

void char_literal_free(CharLiteral** charLiteral) {
    if (charLiteral == NULL || *charLiteral == NULL)
        return;

    free(*charLiteral);
    *charLiteral = NULL;
}

StringLiteral* string_literal_new(const char* value) {
    StringLiteral* type = NULL;
    type = malloc(sizeof(StringLiteral));
    if (type == NULL) {
        return NULL;
    }

    type->value = str_dup(value);

    return type;
}

void string_literal_to_string(StringLiteral** stringLiteral) {
    if (stringLiteral == NULL || *stringLiteral == NULL)
        return;

    printf("\"%s\"", (*stringLiteral)->value);
}

void string_literal_free(StringLiteral** stringLiteral) {
    if (stringLiteral == NULL || *stringLiteral == NULL)
        return;

    free((*stringLiteral)->value);

    free(*stringLiteral);
    *stringLiteral = NULL;
}

BoolLiteral* bool_literal_new(bool value) {
    BoolLiteral* type = NULL;
    type = malloc(sizeof(BoolLiteral));
    if (type == NULL) {
        return NULL;
    }

    type->value = value;

    return type;
}

void bool_literal_to_string(BoolLiteral** boolLiteral) {
    if (boolLiteral == NULL || *boolLiteral == NULL)
        return;

    printf("%s", (*boolLiteral)->value ? "true" : "false");
}

void bool_literal_free(BoolLiteral** boolLiteral) {
    if (boolLiteral == NULL || *boolLiteral == NULL)
        return;

    free(*boolLiteral);
    *boolLiteral = NULL;
}

VoidLiteral* void_literal_new(void) {
    VoidLiteral* type = NULL;
    type = malloc(sizeof(VoidLiteral));
    if (type == NULL) {
        return NULL;
    }

    type->value = NULL;

    return type;
}

void void_literal_to_string(VoidLiteral** voidLiteral) {
    if (voidLiteral == NULL || *voidLiteral == NULL)
        return;

    printf("void");
}

void void_literal_free(VoidLiteral** voidLiteral) {
    if (voidLiteral == NULL || *voidLiteral == NULL)
        return;

    free(*voidLiteral);
    *voidLiteral = NULL;
}

NilLiteral* nil_literal_new(void) {
    NilLiteral* type = NULL;
    type = malloc(sizeof(NilLiteral));
    if (type == NULL) {
        return NULL;
    }

    type->value = NULL;

    return type;
}

void nil_literal_to_string(NilLiteral** nilLiteral) {
    if (nilLiteral == NULL || *nilLiteral == NULL)
        return;

    printf("nil");
}

void nil_literal_free(NilLiteral** nilLiteral) {
    if (nilLiteral == NULL || *nilLiteral == NULL)
        return;

    free(*nilLiteral);
    *nilLiteral = NULL;
}
