#include "types.h"

#include <stdio.h>
#include <stdlib.h>

#include "list.h"
#include "utils.h"
#include "smem.h"


Type* type_new(TypeID typeId, void* type, void (*to_string)(void**), void (*destroy)(void**)) {
    Type* new_type = NULL;
    new_type = safe_malloc(sizeof(Type), NULL);
    if (new_type == NULL) {
        if (destroy != NULL) {
            destroy(&type);
        }
        return NULL;
    }

    *new_type = (Type) {
        .typeId = typeId,
        .type = type,
        .to_string = to_string,
        .destroy = destroy
    };

    return new_type;
}

void type_to_string(Type** type) {
    if (type == NULL || *type == NULL)
        return;

    if ((*type)->to_string != NULL)
        (*type)->to_string(&(*type)->type);
}

void type_free(Type** type) {
    if (type == NULL || *type == NULL)
        return;

    if ((*type)->destroy != NULL)
        (*type)->destroy(&(*type)->type);

    safe_free((void**) type);
}

CommonType* common_type_new(size_t size, char* name) {
    CommonType* type = NULL;
    type = safe_malloc(sizeof(CommonType), NULL);
    if (type == NULL) {
        return NULL;
    }

    *type = (CommonType) {
        .size = size,
        .name = str_dup(name)
    };

    return type;
}

void common_type_to_string(CommonType** commonType) {
    if (commonType == NULL || *commonType == NULL)
        return;

    printf("%s", (*commonType)->name);
}

void common_type_free(CommonType** commonType) {
    if (commonType == NULL || *commonType == NULL)
        return;

    safe_free((void**) &(*commonType)->name);

    safe_free((void**) commonType);
}

NamedType* named_type_new(char* name, Type* type) {
    NamedType* new_type = NULL;
    new_type = safe_malloc(sizeof(NamedType), NULL);
    if (new_type == NULL) {
        type_free(&type);
        return NULL;
    }

    *new_type = (NamedType) {
        .name = str_dup(name),
        .type = type
    };

    return new_type;
}

void named_type_to_string(NamedType** namedType) {
    if (namedType == NULL || *namedType == NULL)
        return;

    printf("%s", (*namedType)->name);

    printf(": ");

    type_to_string(&(*namedType)->type);
}

void named_type_free(NamedType** namedType) {
    if (namedType == NULL || *namedType == NULL)
        return;

    safe_free((void**) &(*namedType)->name);
    type_free(&(*namedType)->type);

    safe_free((void**) namedType);
}

StructType* struct_type_new(size_t size, char* name, List* fields) {
    StructType* type = NULL;
    type = safe_malloc(sizeof(StructType), NULL);
    if (type == NULL) {
        list_free(&fields);
        return NULL;
    }

    *type = (StructType) {
        .size = size,
        .name = str_dup(name),
        .fields = fields
    };

    return type;
}

void struct_type_add_field(StructType** structType, Type* field) {
    if (structType == NULL || *structType == NULL || field == NULL)
        return;

    list_insert_last(&(*structType)->fields, field);
}

void struct_type_to_string(StructType** structType) {
    if (structType == NULL || *structType == NULL)
        return;

    printf("%s{", (*structType)->name);

    List* fields = (*structType)->fields;
    if (!list_is_empty(&fields)) {
        printf(" ");

        list_foreach(field, (*structType)->fields) {
            type_to_string((Type**) &field->value);

            if (field->next != NULL) {
                printf(", ");
            }
        }

        printf(" ");
    }

    printf("}");
}

void struct_type_free(StructType** structType) {
    if (structType == NULL || *structType == NULL)
        return;

    safe_free((void**) &(*structType)->name);
    list_free(&(*structType)->fields);

    safe_free((void**) structType);
}

ArrayDimension* array_dimension_new(size_t size) {
    ArrayDimension* type = NULL;
    type = safe_malloc(sizeof(ArrayDimension), NULL);
    if (type == NULL) {
        return NULL;
    }

    *type = (ArrayDimension) {
        .size = size
    };

    return type;
}
void array_dimension_to_string(ArrayDimension** arrayDimension) {
    if (arrayDimension == NULL || *arrayDimension == NULL)
        return;

    if ((*arrayDimension)->size > 0) {
        printf("[%ld]", (*arrayDimension)->size);
    } else {
        printf("[]");
    }
}

void array_dimension_free(ArrayDimension** arrayDimension) {
    if (arrayDimension == NULL || *arrayDimension == NULL)
        return;

    safe_free((void**) arrayDimension);
}

ArrayType* array_type_new(List* dimensions, Type* type) {
    ArrayType* array_type = NULL;
    array_type = safe_malloc(sizeof(ArrayType), NULL);
    if (array_type == NULL) {
        list_free(&dimensions);
        type_free(&type);
        return NULL;
    }

    *array_type = (ArrayType) {
        .dimensions = dimensions,
        .type = type
    };

    return array_type;
}

void array_type_add_dimension(ArrayType** arrayType, Type* dimension) {
    if (arrayType == NULL || *arrayType == NULL || dimension == NULL)
        return;

    list_insert_last(&(*arrayType)->dimensions, dimension);
}

void array_type_to_string(ArrayType** arrayType) {
    if (arrayType == NULL || *arrayType == NULL)
        return;

    list_foreach(dimension, (*arrayType)->dimensions) {
        type_to_string((Type**) &dimension->value);
    }

    type_to_string(&(*arrayType)->type);
}

void array_type_free(ArrayType** arrayType) {
    if (arrayType == NULL || *arrayType == NULL)
        return;

    list_free(&(*arrayType)->dimensions);
    type_free(&(*arrayType)->type);

    safe_free((void**) arrayType);
}