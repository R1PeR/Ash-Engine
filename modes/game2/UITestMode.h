
#ifndef LIBS_ENGINE_UITESTMODE_H
#define LIBS_ENGINE_UITESTMODE_H
#include "ashes/ash_components.h"
#include "ashes/ash_context.h"
#include "ashes/ash_misc.h"
#include "utils/UI.h"

#include <stdint.h>

#define RENDERSTACKITEM_MAX 4096

#define MAP_TILESET_COLS  24
#define MAP_TILESET_ROWS  16
#define MAP_TILESET_COUNT (MAP_TILESET_COLS * MAP_TILESET_ROWS)

#define TILESET_ATLAS_COLS MAP_TILESET_COLS
#define TILESET_ATLAS_ROWS MAP_TILESET_ROWS
#define TILESET_COUNT      MAP_TILESET_COUNT

#define FONT_ATLAS_COLS  16
#define FONT_ATLAS_ROWS  16
#define FONT_GLYPH_COUNT (FONT_ATLAS_COLS * FONT_ATLAS_ROWS)

struct UITestMode
{
    RenderStack     renderStack;
    RenderStackItem renderStackItems[RENDERSTACKITEM_MAX];
    size_t          renderStackItemCount = 0;

    TextureData tileTextures[TILESET_COUNT];
    TextureData tileAtlasBase;

    TextureData fontTextures[FONT_GLYPH_COUNT];
    TextureData fontAtlasBase;

    Entity2D cameraEntity;
    Sprite   testSprite;

    float       sliderVal  = 0.5f;
    int         listChoice = -1;
    int         btnCount   = 0;
    char        btnLabel[64];
    const char* listItems[16] = { "Apples", "Bananas",   "Cherries",   "Dates",     "Elderberries", "Figs",
                                  "Grapes", "Honeydew",  "Indian Fig", "Jackfruit", "Kiwi",         "Lemon",
                                  "Mango",  "Nectarine", "Orange",     "Papaya" };
    int         listItemCount = sizeof(listItems) / sizeof(listItems[0]);
    UI_State    uiState;
    Window*     window;
    DeltaTime   deltaTime;
    Camera2D    camera;
};

void UITestMode_OnStart(Context* context, void* data);
void UITestMode_OnPause(Context* context, void* data);
void UITestMode_Update(Context* context, void* data);
void UITestMode_Draw(Context* context, void* data);
void UITestMode_OnStop(Context* context, void* data);
void UITestMode_OnResume(Context* context, void* data);

#endif  // LIBS_ENGINE_UITESTMODE_H
