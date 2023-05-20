#include "types.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "utils.h"
#include "smem.h"


static bool compare_list_of_types(List* a, List* b) {
    if (list_size(&a) != list_size(&b))
        return false;

    ListNode* aNode = a->head;
    ListNode* bNode = b->head;
    while (aNode != NULL && bNode != NULL) {
        if (!type_equals((Type**) &aNode->value, (Type**) &bNode->value)) {
            return false;;
        }

        aNode = aNode->next;
        bNode = bNode->next;
    }

    return true;
}

Type* type_new(TypeID typeId, void* type, bool (*equals)(void**, void**), void (*to_string)(void**), void (*destroy)(void**)) {
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
        .equals = equals,
        .to_string = to_string,
        .destroy = destroy
    };

    return new_type;
}

bool type_equals(Type** self, Type** other) {
    if (self == NULL || *self == NULL || other == NULL || *other == NULL)
        return false;

    if ((*self)->equals != NULL)
        return (*self)->equals(&(*self)->type, (void**) other);

    return false;
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

AtomicType* atomic_type_new(size_t size, char* name) {
    AtomicType* type = NULL;
    type = safe_malloc(sizeof(AtomicType), NULL);
    if (type == NULL) {
        return NULL;
    }

    *type = (AtomicType) {
        .size = size,
        .name = str_dup(name)
    };

    return type;
}

static bool is_atomic(Type* type) {
    return _atomic_start < type->typeId && type->typeId < _atomic_end;
}

bool atomic_type_equals(AtomicType** self, Type** other) {
    if (self == NULL || *self == NULL || other == NULL || *other == NULL)
        return false;

    if (!is_atomic(*other) && (*other)->typeId != CUSTOM_TYPE)
        return false;

    const AtomicType* otherAtomicType = (AtomicType*) (*other)->type;

    return strcmp((*self)->name, otherAtomicType->name) == 0;
}

void atomic_type_to_string(AtomicType** atomicType) {
    if (atomicType == NULL || *atomicType == NULL)
        return;

    printf("%s", (*atomicType)->name);
}

void atomic_type_free(AtomicType** atomicType) {
    if (atomicType == NULL || *atomicType == NULL)
        return;

    safe_free((void**) &(*atomicType)->name);

    safe_free((void**) atomicType);
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

bool named_type_equals(NamedType** self, Type** other) {
    if (self == NULL || *self == NULL || other == NULL || *other == NULL)
        return false;

    if ((*other)->typeId != NAMED_TYPE)
        return false;

    NamedType* otherNamedType = (NamedType*) (*other)->type;

    bool hasEqualName = strcmp((*self)->name, otherNamedType->name) == 0;
    bool hasEqualType = type_equals(&(*self)->type, &otherNamedType->type);

    return hasEqualName && hasEqualType;
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

bool struct_type_equals(StructType** self, Type** other) {
    if (self == NULL || *self == NULL || other == NULL || *other == NULL)
        return false;

    if ((*other)->typeId != STRUCT_TYPE)
        return false;

    StructType* otherStructType = (StructType*) (*other)->type;

    return compare_list_of_types((*self)->fields, otherStructType->fields);
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

bool array_dimension_equals(ArrayDimension** self, Type** other) {
    if (self == NULL || *self == NULL || other == NULL || *other == NULL)
        return false;

    if ((*other)->typeId != ARRAY_DIMENSION_TYPE)
        return false;

    ArrayDimension* otherArrayDimension = (ArrayDimension*) (*other)->type;

    return (*self)->size == otherArrayDimension->size;
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

bool array_type_equals(ArrayType** self, Type** other) {
    if (self == NULL || *self == NULL || other == NULL || *other == NULL)
        return false;

    if ((*other)->typeId != ARRAY_TYPE)
        return false;

    ArrayType* otherArrayType = (ArrayType*) (*other)->type;

    if (!type_equals(&(*self)->type, &otherArrayType->type))
        return false;

    return compare_list_of_types((*self)->dimensions, otherArrayType->dimensions);
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

FunctionType* function_type_new(List* parameterTypes, Type* returnType) {
    FunctionType* type = NULL;
    type = safe_malloc(sizeof(FunctionType), NULL);
    if (type == NULL) {
        list_free(&parameterTypes);
        type_free(&returnType);
        return NULL;
    }

    *type = (FunctionType) {
        .parameterTypes = parameterTypes,
        .returnType = returnType
    };

    return type;
}

void function_type_add_parameter(FunctionType** functionType, Type* parameter) {
    if (functionType == NULL || *functionType == NULL || parameter == NULL)
        return;

    list_insert_last(&(*functionType)->parameterTypes, parameter);
}

bool function_type_equals(FunctionType** self, Type** other) {
    if (self == NULL || *self == NULL || other == NULL || *other == NULL)
        return false;

    if ((*other)->typeId != FUNC_TYPE)
        return false;

    FunctionType* otherFunctionType = (FunctionType*) (*other)->type;

    Type* voidType = NEW_VOID_TYPE();

    bool selfReturnIsVoid = (*self)->returnType == NULL
                        || type_equals(&(*self)->returnType, &voidType);

    bool otherReturnIsVoid = otherFunctionType->returnType == NULL
                        || type_equals(&otherFunctionType->returnType, &voidType);

    type_free(&voidType);

    bool hasEqualParameters = compare_list_of_types((*self)->parameterTypes, otherFunctionType->parameterTypes);

    if (hasEqualParameters && selfReturnIsVoid && otherReturnIsVoid)
        return true;

    bool hasEqualReturns = ((*self)->returnType != NULL && otherFunctionType->returnType != NULL) ||
        type_equals(&(*self)->returnType, &otherFunctionType->returnType);

    return hasEqualParameters && hasEqualReturns;
}

void function_type_to_string(FunctionType** functionType) {
    if (functionType == NULL || *functionType == NULL)
        return;

    printf("func(");

    list_foreach(parameter, (*functionType)->parameterTypes) {
        type_to_string((Type**) &parameter->value);

        if (parameter->next != NULL) {
            printf(", ");
        }
    }

    printf(")");

    if ((*functionType)->returnType != NULL) {
        printf(": ");

        type_to_string(&(*functionType)->returnType);
    }
}

void function_type_free(FunctionType** functionType) {
    if (functionType == NULL || *functionType == NULL)
        return;

    list_free(&(*functionType)->parameterTypes);
    type_free(&(*functionType)->returnType);

    safe_free((void**) functionType);
}
