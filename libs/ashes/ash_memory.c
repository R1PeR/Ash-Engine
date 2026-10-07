
#include "ash_memory.h"

#include "ash_debug.h"

#include <stdlib.h>

Arena* Arena_Create(size_t size)
{
    Arena* arena = (Arena*)malloc(sizeof(Arena));
    if (!arena)
    {
        LOG_ERR("Arena creation failed: Unable to allocate memory for Arena structure.");
        return NULL;
    }
    arena->buffer = (uint8_t*)malloc(size);
    if (!arena->buffer)
    {
        LOG_ERR("Arena creation failed: Unable to allocate memory for buffer.");
        free(arena);
        return NULL;
    }
    arena->size = size;
    arena->used = 0;
    return arena;
}

void Arena_Init(Arena* arena, size_t size)
{
    arena->buffer = (uint8_t*)malloc(size);
    arena->size   = size;
}

void Arena_Release(Arena* arena)
{
    // Reset the arena by setting the size to 0
    arena->used = 0;
}

void Arena_Free(Arena* arena)
{
    free(arena->buffer);
    arena->buffer = NULL;
    arena->size   = 0;
    arena->used   = 0;
    free(arena);
}

void* Arena_Allocate(Arena* arena, size_t size)
{
    if (arena->used + size > arena->size)
    {
        // Not enough space in the arena
        LOG_ERR("Arena allocation failed: Not enough space in the arena.");
        return NULL;
    }
    arena->used += size;
    return arena->buffer + arena->used - size;
}

size_t Arena_GetSize(Arena* arena)
{
    return arena->size;
}

size_t Arena_GetUsed(Arena* arena)
{
    return arena->used;
}

Arena Arena_ScratchCreate(Arena* arena)
{
    Arena scratch;
    scratch.buffer = arena->buffer + arena->used;
    scratch.size   = arena->size - arena->used;
    scratch.used   = 0;
    arena->used += scratch.size;
    return scratch;
}

void Arena_ScratchFree(Arena* arena, Arena* scratch)
{
    scratch->used   = 0;
    scratch->buffer = NULL;
    scratch->size   = 0;
    arena->used -= scratch->size;
}
