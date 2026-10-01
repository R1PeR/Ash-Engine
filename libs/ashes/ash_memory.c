
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

// #define Arena_Allocate(arena, size)                                                                                  \
//     ((arena)->used + (size) <= (arena)->size ? ((arena)->used += (size), (arena)->buffer + (arena)->used - (size)) : \
//                                                NULL)

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
