#include "utils.h"

#include <stdlib.h>
#include <memory.h>


char* str_dup(const char* other) {
    char* copy = NULL;
    size_t len = strlen(other);

    copy = malloc(len + 1);
    memcpy(copy, other, len);
    copy[len] = '\0';

    return copy;
}
