#pragma once

#include <stdbool.h>

#include "map.h"


typedef struct Context {
    Map* environment;
    struct Context* enclosing;
} Context;

Context* context_new(Map* environment);
Context* context_enclosed_new(Context* enclosing, Map* environment);
void context_free(Context** ctx);

void context_define(Context* ctx, void* name, void* value);
void* context_get(Context* ctx, void* name);
void context_assign(Context* ctx, void* name, void* value);
bool context_exists(Context* ctx, void* name);
