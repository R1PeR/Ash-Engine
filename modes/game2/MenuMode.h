
#ifndef LIBS_ENGINE_MENUMODE_H
#define LIBS_ENGINE_MENUMODE_H
#include "ashes/ash_components.h"
#include "ashes/ash_context.h"
#include "utils/ArenaDebugFrame.h"
#include "utils/UI.h"

#define RENDERSTACKITEM_MAX 4096
#define FONT_ATLAS_COLS     16
#define FONT_ATLAS_ROWS     16
#define FONT_GLYPH_COUNT    (FONT_ATLAS_COLS * FONT_ATLAS_ROWS)

struct MenuMode
{
    RenderStack     renderStack;
    RenderStackItem renderStackItems[RENDERSTACKITEM_MAX];
    size_t          renderStackItemCount = 0;
    TextureData     fontTextures[FONT_GLYPH_COUNT];
    TextureData     fontAtlasBase;
    Entity2D        cameraEntity;
    UI_State        uiState;
    Window*         window;
    Arena*          arena;
    DeltaTime       deltaTime;
    Camera2D        camera;
    ArenaDebug      debugArena;
};

void MenuMode_OnStart(Context* context, void* data);
void MenuMode_OnPause(Context* context, void* data);
void MenuMode_Update(Context* context, void* data);
void MenuMode_Draw(Context* context, void* data);
void MenuMode_OnStop(Context* context, void* data);
void MenuMode_OnResume(Context* context, void* data);

#endif
