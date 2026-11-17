#ifndef ASH_MEMORY_H
#define ASH_MEMORY_H
#include <stdint.h>

#define B(x)  (x)
#define KB(x) (B(x) * 1024)
#define MB(x) (KB(x) * 1024)
#define GB(x) (MB(x) * 1024)

typedef struct ArenaData ArenaData;

typedef struct Arena Arena;
typedef struct Arena
{
    uint8_t* buffer;
    size_t   size;
    size_t   used;
    Arena*   debug;
    bool     isScratch;
} Arena;

typedef struct ArenaData
{
    uint8_t* recordBuffer;
    size_t   recordSize;
} ArenaData;

Arena* Arena_Create(size_t size);
void   Arena_Init(Arena* arena, size_t size);
void   Arena_Release(Arena* arena);
void   Arena_Free(Arena* arena);
size_t Arena_GetSize(Arena* arena);
size_t Arena_GetUsed(Arena* arena);

// Creates a scratch arena from the given arena. The scratch arena will use the remaining space in the given arena. The
// scratch arena should be released using Arena_ScratchFree when done.
Arena Arena_ScratchCreate(Arena* arena);
void  Arena_ScratchFree(Arena* scratch);

void* Arena_Allocate(Arena* arena, size_t size);
#define Arena_Push(type, arena)             ((type*)Arena_Allocate((arena), (sizeof(type))))
#define Arena_PushArray(type, arena, count) ((type*)Arena_Allocate((arena), (sizeof(type) * (count))))

#endif  // ASH_MEMORY_H
