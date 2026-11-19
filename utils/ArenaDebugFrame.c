#include "ArenaDebugFrame.h"

#include "ashes/ash_debug.h"

#include <stdio.h>

void ArenaDebugFrame_Init(ArenaDebug* debug, uint32_t maxRecords)
{
    debug->maxRecords = maxRecords;
    debug->tempArena  = Arena_Create(KB(4));
    debug->buffer     = NULL;
    debug->graphData  = NULL;
}
void ArenaDebugFrame_Destroy(ArenaDebug* debug)
{
    Arena_Free(debug->tempArena);
}

void ArenaDebugFrame_DisplayFrame(ArenaDebug* debug, UI_State* ui, TextureData* font, Arena* arena)
{
    if (arena->debug == NULL)
    {
        LOG_ERR("ArenaDebugFrame_DisplayFrame: arena->debug is NULL. Make sure to initialize the debug arena using "
                "ArenaDebugFrame_Init.");
        return;
    }
    if (debug->tempArena == NULL)
    {
        LOG_ERR("ArenaDebugFrame_DisplayFrame: temporary arena is NULL.");
        return;
    }
    Arena_Release(debug->tempArena);
    debug->buffer    = Arena_PushArray(char*, debug->tempArena, debug->maxRecords);
    debug->graphData = Arena_PushArray(float, debug->tempArena, debug->maxRecords);
    if (debug->buffer == NULL || debug->graphData == NULL)
    {
        LOG_ERR("ArenaDebugFrame_DisplayFrame: unable to allocate record arrays.");
        return;
    }
    size_t     dataCount = arena->debug->used / sizeof(ArenaData);
    ArenaData* current   = (ArenaData*)arena->debug->buffer;
    uint16_t   n         = 0;
    for (size_t i = 0; i < dataCount; i++)
    {
        if (n >= debug->maxRecords)
            break;
        int len = snprintf(NULL, 0, "Size: %zu, Pos: %td", current->recordSize, current->recordBuffer - arena->buffer);
        if (len < 0)
        {
            LOG_ERR("ArenaDebugFrame_DisplayFrame: unable to format record.");
            return;
        }
        debug->buffer[n] = (char*)Arena_PushArray(char, debug->tempArena, len + 1);
        if (debug->buffer[n] == NULL)
        {
            LOG_ERR("ArenaDebugFrame_DisplayFrame: unable to allocate record text.");
            return;
        }
        snprintf(debug->buffer[n], len + 1, "Size: %zu, Pos: %td", current->recordSize,
                 current->recordBuffer - arena->buffer);
        debug->graphData[n] = (float)current->recordSize;
        current             = (ArenaData*)((uint8_t*)current + sizeof(ArenaData));
        n++;
    }
    char currentUsage[64];
    snprintf(currentUsage, sizeof(currentUsage), "Usage: %3.1f%%",
             (float)((float)(arena->used) * 100.0f / (float)(arena->size)));

    UI_Begin(ui, UI_GetBounds(ui, AnchorTopRight, { 0.0f, 0.0f, 0.4f, 0.3f }));
    {
        UI_Frame(ui);
        {
            UI_Center(ui, CenterBoth);
            UI_Size(ui, 0.1f);
            UI_Text(ui, "Arena Debug Frame", 1.0f, font);
        }
        UI_Frame(ui);
        {
            UI_Size(ui, 0.4f);
            UI_Graph1D(ui, debug->graphData, n, Graph1dVertical);
        }
        UI_Frame(ui);
        {
            UI_Center(ui, CenterBoth);
            UI_Size(ui, 0.1f);
            UI_Text(ui, currentUsage, 1.0f, font);
        }
        UI_Frame(ui);
        {
            UI_Padding(ui, UI_GetSize(ui, { 0.01f, 0.01f, 0.01f, 0.01f }));
            UI_Size(ui, 0.4f);
            UI_List(ui, (const char**)debug->buffer, n, 1.0f, 20.0f, font, false);
        }
    }
    UI_End(ui);
}
