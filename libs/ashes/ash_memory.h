#ifndef ASH_MEMORY_H
#define ASH_MEMORY_H
#include <stdint.h>

typedef struct Arena
{
    uint8_t* buffer;
    size_t   size;
    size_t   used;
} Arena;

void Arena_Init(Arena* arena, uint8_t* buffer, size_t size);
void Arena_Reset(Arena* arena);
void Arena_Destroy(Arena* arena);
void*  Arena_Allocate(Arena* arena, size_t size);
size_t Arena_GetSize(Arena* arena);
size_t Arena_GetUsed(Arena* arena);

#endif  // ASH_MEMORY_H
