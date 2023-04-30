#include "types.h"

#include <stdio.h>
#include <stdlib.h>

#include "utils.h"


Type* type_new(TypeID id, size_t size, char* name, List* fields) {
    Type* new_type = NULL;
    new_type = malloc(sizeof(Type));
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

void type_add_field(Type** type, Type* field) {
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
            type_to_string((Type**) &field->value);

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

    free((*type)->name);
    list_free(&(*type)->fields);

    free(*type);
    *type = NULL;
}
