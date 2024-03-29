#include "utils.h"

#include <memory.h>

#include "smem.h"


char* str_dup(const char* other) {
    size_t len = strlen(other);

    char* copy = NULL;
    copy = safe_malloc(len + 1, NULL);
    if (copy == NULL) {
        return "";
    }

    memcpy(copy, other, len);
    copy[len] = '\0';

    return copy;
}

void remove_quotes(char** str) {
    size_t len = strlen(*str);

    if (len >= 2 && ((*str)[0] == '"' ||
        (*str)[0] == '\'') && ((*str)[len-1] == '"' ||
        (*str)[len-1] == '\'')
    ) {
        memmove(*str, *str + 1, len - 2);
        (*str)[len - 2] = '\0';
    }
}

char* escape_special_chars(const char* str) {
    size_t str_len = strlen(str);
    char* escaped_str = NULL;
    escaped_str = safe_malloc((2 * str_len + 1) * sizeof(char), NULL);
    size_t j = 0;

    for (size_t i = 0; i < str_len; i++) {
        switch (str[i]) {
        case '\n':
            escaped_str[j++] = '\\';
            escaped_str[j++] = 'n';
            break;
        case '\t':
            escaped_str[j++] = '\\';
            escaped_str[j++] = 't';
            break;
        case '\r':
            escaped_str[j++] = '\\';
            escaped_str[j++] = 'r';
            break;
        case '\f':
            escaped_str[j++] = '\\';
            escaped_str[j++] = 'f';
            break;
        case '\v':
            escaped_str[j++] = '\\';
            escaped_str[j++] = 'v';
            break;
        case '\'':
            escaped_str[j++] = '\\';
            escaped_str[j++] = '\'';
            break;
        case '\"':
            escaped_str[j++] = '\\';
            escaped_str[j++] = '\"';
            break;
        case '\\':
            escaped_str[j++] = '\\';
            escaped_str[j++] = '\\';
            break;
        default:
            escaped_str[j++] = str[i];
            break;
        }
    }

    escaped_str[j] = '\0';
    return escaped_str;
}

unsigned long hash_int(int value) {
    unsigned long y = (unsigned long) value;

    y = (y + 0x7ed55d16UL) + (y << 12);
    y = (y ^ 0xc761c23cUL) ^ (y >> 19);
    y = (y + 0x165667b1UL) + (y << 5);
    y = (y + 0xd3a2646cUL) ^ (y << 9);
    y = (y + 0xfd7046c5UL) + (y << 3);
    y = (y ^ 0xb55a4f09UL) ^ (y >> 16);

    return y;
}

unsigned long hash_float(float value) {
    union {
        float f;
        unsigned long ul;
    } u;

    u.f = value;

    return hash_int(u.ul);
}

unsigned long hash_double(double value) {
    union {
        double d;
        unsigned long u;
    } u = { .d = value };

    unsigned long y = (u.u + 0x7ed55d16UL) + (u.u << 12);

    y = (y ^ 0xc761c23cUL) ^ (y >> 19);
    y = (y + 0x165667b1UL) + (y << 5);
    y = (y + 0xd3a2646cUL) ^ (y << 9);
    y = (y + 0xfd7046c5UL) + (y << 3);
    y = (y ^ 0xb55a4f09UL) ^ (y >> 16);

    return y + 0x9e3779b9UL;
}

unsigned long hash_char(char value) {
    unsigned long y = (unsigned long) value;

    y = (y + 0x7ed55d16UL) + (y << 12);
    y = (y ^ 0xc761c23cUL) ^ (y >> 19);
    y = (y + 0x165667b1UL) + (y << 5);
    y = (y + 0xd3a2646cUL) ^ (y << 9);
    y = (y + 0xfd7046c5UL) + (y << 3);
    y = (y ^ 0xb55a4f09UL) ^ (y >> 16);

    return y;
}

unsigned long hash_string(const char* value) {
    unsigned long hash = 5381;
    unsigned char c;

    while ((c = *value++) != 0) {
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}
