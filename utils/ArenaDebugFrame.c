#include "ArenaDebugFrame.h"

#include <stdio.h>

void ArenaDebugFrame_DisplayFrame(UI_State* ui, TextureData* font, Arena* arena)
{
//     const uint16_t maxRecords = 32;
//     Arena*         tempArena  = Arena_Create(KB(4));
//     char**         buffer     = (char**)Arena_PushArray(char*, tempArena, maxRecords);
//     float*         graphData  = (float*)Arena_PushArray(float, tempArena, maxRecords);
//     uint16_t       n          = 0;
//     if (arena->debug)
//     {
//         uint16_t   dataCount = arena->debug->used / sizeof(ArenaData);
//         ArenaData* current   = (ArenaData*)arena->debug->buffer;
//         for (int i = 0; i < dataCount; i++)
//         {
//             if (n >= maxRecords)
//                 break;
//             uint16_t len = snprintf(NULL, 0, "Size: %zu, Used: %zu", current->recordSize, arena->used);
//             buffer[n]    = (char*)Arena_PushArray(char, tempArena, len + 1);
//             snprintf(buffer[n], len + 1, "Size: %zu, Pos: %zu", current->recordSize,
//                      current->recordBuffer - arena->buffer);
//             graphData[n] = (float)current->recordSize;
//             current      = (ArenaData*)((uint8_t*)current + sizeof(ArenaData));
//             n++;
//         }
//     }
//     UI_Begin(ui, UI_GetBounds(ui, AnchorTopRight, { 0.0f, 0.0f, 0.4f, 0.3f }));
//     UI_Frame(ui);
//     {
//         UI_Center(ui, CenterHorizontal);
//         UI_Text(ui, "Arena Debug Frame", 1.0f, font);
//         UI_Graph1D(ui, (float*)&arena->used, n, Graph1dVertical);
//         UI_List(ui, (const char**)buffer, n, 1.0f, 20.0f, font, false);
//     }
//     UI_End(ui);
//     Arena_Free(tempArena);
}
