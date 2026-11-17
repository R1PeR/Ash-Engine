
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
    LOG_DBG("Arena created: Allocated %zu bytes for arena at %p.", size, (void*)arena);
    arena->size = size;
    arena->used = 0;
    return arena;
}

void Arena_Init(Arena* arena, size_t size)
{
    arena->buffer    = (uint8_t*)malloc(size);
    arena->size      = size;
    arena->used      = 0;
    arena->debug     = NULL;
    arena->isScratch = false;
    LOG_DBG("Arena initialized: Allocated %zu bytes for arena at %p.", size, (void*)arena);
}

void Arena_Release(Arena* arena)
{
    // Reset the arena by setting the size to 0
    arena->used = 0;
    LOG_DBG("Arena released: Memory reset for arena at %p.", (void*)arena);
}

void Arena_Free(Arena* arena)
{
    free(arena->buffer);
    arena->buffer = NULL;
    arena->size   = 0;
    arena->used   = 0;
    LOG_DBG("Arena freed: Memory released for arena at %p.", (void*)arena);
}

void* Arena_Allocate(Arena* arena, size_t size)
{
    if (arena->used + size > arena->size)
    {
        // Not enough space in the arena
        LOG_ERR("Arena allocation failed: Not enough space in the arena.");
        return NULL;
    }
    uint8_t* startAddress = arena->buffer + arena->used;

    if (arena->debug != NULL && arena->isScratch == false)
    {
        ArenaData* currentData = (ArenaData*)(arena->debug->buffer + arena->debug->used);
        arena->debug->used += sizeof(ArenaData);
        currentData->recordBuffer = startAddress;
        currentData->recordSize   = size;
    }

    if (!arena->isScratch)
    {
        LOG_DBG("Arena allocation: Allocated %zu bytes from arena at %p.", size, (void*)arena);
    }

    arena->used += size;
    return startAddress;
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
    scratch.buffer    = arena->buffer + arena->used;
    scratch.size      = arena->size - arena->used;
    scratch.used      = 0;
    scratch.debug     = arena->debug;
    scratch.isScratch = true;
    return scratch;
}

void Arena_ScratchFree(Arena* scratch)
{
    scratch->used   = 0;
    scratch->buffer = NULL;
    scratch->debug  = NULL;
    scratch->size   = 0;
}
