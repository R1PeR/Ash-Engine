#ifndef ASH_MEMORY_H
#define ASH_MEMORY_H
#include <stdint.h>

typedef struct Arena
{
    uint8_t* buffer;
    size_t   size;
    size_t   used;
} Arena;

Arena* Arena_Create(size_t size);
void   Arena_Init(Arena* arena, size_t size);
void   Arena_Release(Arena* arena);
void   Arena_Free(Arena* arena);
size_t Arena_GetSize(Arena* arena);
size_t Arena_GetUsed(Arena* arena);

void* Arena_Allocate(Arena* arena, size_t size);
#define Arena_Push(type, arena)             ((type*)Arena_Allocate((arena), (sizeof(type))))
#define Arena_PushArray(type, arena, count) ((type*)Arena_Allocate((arena), (sizeof(type) * (count))))

#endif  // ASH_MEMORY_H
