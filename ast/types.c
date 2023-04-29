#include "types.h"

#include <stdio.h>
#include <stdlib.h>

#include "utils.h"


Type* type_new(TypeID id, size_t size, char* name) {
    Type* new_type = NULL;
    new_type = malloc(sizeof(Type));
    if (new_type == NULL) {
        return NULL;
    }

    *new_type = (Type) {
        .id = id,
        .size = size,
        .name = str_dup(name)
    };

    return new_type;
}

void type_to_string(Type** type) {
    if (type == NULL || *type == NULL)
        return;

    printf("%s", (*type)->name);
}

void type_free(Type** type) {
    if (type == NULL || *type == NULL)
        return;

    free((*type)->name);

    free(*type);
    *type = NULL;
}
