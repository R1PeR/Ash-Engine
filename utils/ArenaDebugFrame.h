#ifndef ARENA_DEBUG_FRAME_H
#define ARENA_DEBUG_FRAME_H
#include "UI.h"
#include "ashes/ash_memory.h"
struct ArenaDebug
{
    Arena*   tempArena;
    char**   buffer;
    float*   graphData;
    uint16_t maxRecords;
};

void ArenaDebugFrame_Init(ArenaDebug* debug, uint32_t maxRecords);
void ArenaDebugFrame_Destroy(ArenaDebug* debug);
void ArenaDebugFrame_DisplayFrame(ArenaDebug* debug, UI_State* ui, TextureData* font, Arena* arena);

#endif
