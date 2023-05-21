#include "context.h"

#include "map.h"
#include "smem.h"


Context* context_new(Map* environment) {
    Context* new_ctx = NULL;
    new_ctx = safe_calloc(1, sizeof(Context), NULL);
    if (new_ctx == NULL) {
        map_free(&environment);
        return NULL;
    }

    *new_ctx = (Context) {
        .environment = environment,
        .enclosing = NULL
    };

    return new_ctx;
}

Context* context_enclosed_new(Context* enclosing, Map* environment) {
    Context* new_ctx = NULL;
    new_ctx = safe_calloc(1, sizeof(Context), NULL);
    if (new_ctx == NULL) {
        context_free(&enclosing);
        map_free(&environment);
        return NULL;
    }

    *new_ctx = (Context) {
        .enclosing = enclosing,
        .environment = environment
    };

    return new_ctx;
}

void context_free(Context** ctx) {
    if (ctx == NULL || *ctx == NULL)
        return;

    map_free(&(*ctx)->environment);

    safe_free((void**) ctx);
}

void context_define(Context* ctx, void* name, void* value) {
    if (ctx == NULL || name == NULL)
        return;

    map_put(ctx->environment, name, value);
}

void* context_get(Context* ctx, void* name) {
    if (ctx == NULL || name == NULL)
        return NULL;

    void* value = map_get(ctx->environment, name);
    if (value != NULL) {
        return value;
    }

    if (ctx->enclosing != NULL) {
        return context_get(ctx->enclosing, name);
    }

    return NULL;
}

void context_assign(Context* ctx, void* name, void* value) {
    if (ctx == NULL || name == NULL)
        return;

    const void* current_value = map_get(ctx->environment, name);
    if (current_value != NULL) {
        map_put(ctx->environment, name, value);
        return;
    }

    if (ctx->enclosing != NULL) {
        context_assign(ctx->enclosing, name, value);
        return;
    }
}

bool context_exists(Context* ctx, void* name) {
    if (ctx == NULL || name == NULL)
        return false;

    return map_get(ctx->environment, name) != NULL;
}
