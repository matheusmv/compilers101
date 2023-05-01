#include "types.h"

#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include "smem.h"


Type* type_new(TypeID id, size_t size, char* name, List* fields) {
    Type* new_type = NULL;
    new_type = safe_malloc(sizeof(Type), NULL);
    if (new_type == NULL) {
        list_free(&fields);
        return NULL;
    }

    *new_type = (Type) {
        .id = id,
        .size = size,
        .name = str_dup(name),
        .fields = fields
    };

    return new_type;
}

void type_add_field(Type** type, NamedType* field) {
    if (type == NULL || *type == NULL || field == NULL)
        return;

    list_insert_last(&(*type)->fields, field);
}

void type_to_string(Type** type) {
    if (type == NULL || *type == NULL)
        return;

    printf("%s", (*type)->name);

    List* fields = (*type)->fields;
    if (fields != NULL) {
        printf(" { ");

        for (ListNode* field = fields->head; field != NULL; field = field->next) {
            named_type_to_string((NamedType**) &field->value);

            if (field->next != NULL) {
                printf(", ");
            }
        }

        printf(" }");
    }
}

void type_free(Type** type) {
    if (type == NULL || *type == NULL)
        return;

    safe_free((void**) &(*type)->name);
    list_free(&(*type)->fields);

    safe_free((void**) type);
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
