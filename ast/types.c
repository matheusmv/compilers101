#include "types.h"


IndentType* ident_type_new(const char* ident) {
    IndentType* type = NULL;
    type = malloc(sizeof(IndentType));
    if (type == NULL) {
        return NULL;
    }

    type->value = str_dup(ident);

    return type;
}

void ident_type_to_string(IndentType** identType) {
    if (identType == NULL || *identType == NULL)
        return;

    printf("&[%s]", (*identType)->value);
}

void ident_type_free(IndentType** identType) {
    if (identType == NULL || *identType == NULL)
        return;

    free((*identType)->value);

    free(*identType);
    *identType = NULL;
}

IntType* int_type_new(int value) {
    IntType* type = NULL;
    type = malloc(sizeof(IntType));
    if (type == NULL) {
        return NULL;
    }

    type->value = value;

    return type;
}

void int_type_to_string(IntType** intType) {
    if (intType == NULL || *intType == NULL)
        return;

    printf("%d", (*intType)->value);
}

void int_type_free(IntType** intType) {
    if (intType == NULL || *intType == NULL)
        return;

    free(*intType);
    *intType = NULL;
}

FloatType* float_type_new(double value) {
    FloatType* type = NULL;
    type = malloc(sizeof(FloatType));
    if (type == NULL) {
        return NULL;
    }

    type->value = value;

    return type;
}

void float_type_to_string(FloatType** floatType) {
    if (floatType == NULL || *floatType == NULL)
        return;

    printf("%f", (*floatType)->value);
}

void float_type_free(FloatType** floatType) {
    if (floatType == NULL || *floatType == NULL)
        return;

    free(*floatType);
    *floatType = NULL;
}

CharType* char_type_new(char value) {
    CharType* type = NULL;
    type = malloc(sizeof(CharType));
    if (type == NULL) {
        return NULL;
    }

    type->value = value;

    return type;
}

void char_type_to_string(CharType** charType) {
    if (charType == NULL || *charType == NULL)
        return;

    printf("'%c'", (*charType)->value);
}

void char_type_free(CharType** charType) {
    if (charType == NULL || *charType == NULL)
        return;

    free(*charType);
    *charType = NULL;
}

StringType* string_type_new(const char* value) {
    StringType* type = NULL;
    type = malloc(sizeof(StringType));
    if (type == NULL) {
        return NULL;
    }

    type->value = str_dup(value);

    return type;
}

void string_type_to_string(StringType** stringType) {
    if (stringType == NULL || *stringType == NULL)
        return;

    printf("\"%s\"", (*stringType)->value);
}

void string_type_free(StringType** stringType) {
    if (stringType == NULL || *stringType == NULL)
        return;

    free((*stringType)->value);

    free(*stringType);
    *stringType = NULL;
}

BoolType* bool_type_new(bool value) {
    BoolType* type = NULL;
    type = malloc(sizeof(BoolType));
    if (type == NULL) {
        return NULL;
    }

    type->value = value;

    return type;
}

void bool_type_to_string(BoolType** boolType) {
    if (boolType == NULL || *boolType == NULL)
        return;

    printf("%s", (*boolType)->value ? "true" : "false");
}

void bool_type_free(BoolType** boolType) {
    if (boolType == NULL || *boolType == NULL)
        return;

    free(*boolType);
    *boolType = NULL;
}
