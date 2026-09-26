
#include "ash_memory.h"

#include "ash_debug.h"

#include <stdlib.h>

void Arena_Init(Arena* arena, size_t size)
{
    arena->buffer = (uint8_t*)malloc(size);
    arena->size   = size;
}

void Arena_Reset(Arena* arena)
{
    // Reset the arena by setting the size to 0
    arena->used = 0;
}

void Arena_Destroy(Arena* arena)
{
    free(arena->buffer);
    arena->buffer = NULL;
    arena->size   = 0;
    arena->used   = 0;
}

void Arena_Allocate(Arena* arena, size_t size, void** outPtr)
{
    if (arena->used + size > arena->size)
    {
        // Not enough space in the arena
        *outPtr = NULL;
        LOG_ERR("Arena allocation failed: Not enough space in the arena.");
        return;
    }

    *outPtr = arena->buffer + arena->used;
    arena->used += size;
}

size_t Arena_GetSize(Arena* arena)
{
    return arena->size;
}

size_t Arena_GetUsed(Arena* arena)
{
    return arena->used;
}
