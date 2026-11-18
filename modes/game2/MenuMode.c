#include "MenuMode.h"

#include "MainMode.h"
#include "MapEditorMode.h"
#include "ashes/ash_components.h"
#include "ashes/ash_context.h"
#include "ashes/ash_debug.h"
#include "ashes/ash_io.h"
#include "ashes/ash_misc.h"

#include <stdio.h>

void MenuMode_OnStart(Context* context, void* data)
{
    (void)context;
    MenuMode* state      = (MenuMode*)data;
    state->fontAtlasBase = Texture_LoadTexture("resources/sprites/Anikki_square_8x8.png", state->arena);
    if (!Texture_CreateTextureAtlas(state->fontAtlasBase, FONT_ATLAS_COLS, FONT_ATLAS_ROWS, state->fontTextures))
    {
        LOG_ERR("MenuMode: failed to create font atlas");
    }

    Entity2D_Initialize(&state->cameraEntity);

    state->camera.zoom     = 1.0f;
    state->camera.position = (Vector2Float){ 0.0f, 0.0f };
    state->camera.rotation = 0.0f;
    state->camera.offset   = (Vector2Float){ state->window->width / 2.0f, state->window->height / 2.0f };

    state->cameraEntity.position.x = state->camera.position.x;
    state->cameraEntity.position.y = state->camera.position.y;
    state->cameraEntity.scale      = 1.0f / state->camera.zoom;

    UI_Initialize(&state->uiState, &state->renderStack, state->window, &state->camera);
    UI_SetParentEntity(&state->uiState, &state->cameraEntity);

    RenderStack_Initialize(&state->renderStack, state->renderStackItems, STACKITEM_MAX);

    ArenaDebugFrame_Init(&state->debugArena, 32);
}

void MenuMode_OnPause(Context* context, void* data)
{
    (void)context;
    (void)data;
}

void MenuMode_Update(Context* context, void* data)
{
    MenuMode* state             = (MenuMode*)data;
    state->renderStackItemCount = 0;
    DeltaTime_Update(&state->deltaTime);

    RenderStack_Clear(&state->renderStack);
    RenderStack_PushClearScreen(&state->renderStack, (Color){ 0, 0, 0, 255 });
    RenderStack_PushCamera2d(&state->renderStack, &state->camera);
    RenderStack_PushMode2d(&state->renderStack);

    UI_Begin(&state->uiState, UI_GetBounds(&state->uiState, AnchorTopCenter, { 0.5, 0.0, 1.0, 1.0 }));
    {
        UI_Frame(&state->uiState);
        {
            UI_Layout(&state->uiState, LayoutVertical);
            UI_Center(&state->uiState, CenterBoth);
            UI_Text(&state->uiState, "MAIN MENU", 3.0f, state->fontTextures);
        }
        UI_Frame(&state->uiState);
        {
            UI_Layout(&state->uiState, LayoutVertical);
            UI_Center(&state->uiState, CenterBoth);
            UI_Padding(&state->uiState, UI_GetSize(&state->uiState, { 0.01, 0.01, 0.01, 0.01 }));
            if (UI_Button(&state->uiState, "Start Game", 2.0f, state->fontTextures))
            {
                Arena     arena    = Arena_ScratchCreate(state->arena);
                MainMode* mainMode = Arena_Push(MainMode, &arena);
                mainMode->window   = state->window;
                mainMode->arena    = &arena;
                Context_SetMode(context, MODE_FROM_CLASSNAME(MainMode), mainMode);
            }
            if (UI_Button(&state->uiState, "Map Editor", 2.0f, state->fontTextures))
            {
                Arena          arena         = Arena_ScratchCreate(state->arena);
                MapEditorMode* mapEditorMode = Arena_Push(MapEditorMode, &arena);
                mapEditorMode->window        = state->window;
                mapEditorMode->arena         = &arena;
                Context_SetMode(context, MODE_FROM_CLASSNAME(MapEditorMode), mapEditorMode);
            }
            if (UI_Button(&state->uiState, "Exit", 2.0f, state->fontTextures))
            {
                Context_FinishMode(context);
            }
        }
    }
    UI_End(&state->uiState);

    ArenaDebugFrame_DisplayFrame(&state->debugArena, &state->uiState, state->fontTextures, state->arena);

    RenderStack_PopMode2d(&state->renderStack);
    RenderStack_PopCamera2d(&state->renderStack);
}

void MenuMode_Draw(Context* context, void* data)
{
    (void)context;
    MenuMode* state = (MenuMode*)data;
    RenderStack_Draw(&state->renderStack);
}

void MenuMode_OnStop(Context* context, void* data)
{
    (void)context;
    (void)data;
    ArenaDebugFrame_Destroy(&((MenuMode*)data)->debugArena);
}

void MenuMode_OnResume(Context* context, void* data)
{
    (void)context;
    MenuMode* state        = (MenuMode*)data;
    state->camera.zoom     = 1.0f;
    state->camera.position = (Vector2Float){ 0.0f, 0.0f };
    state->camera.rotation = 0.0f;
    state->camera.offset   = (Vector2Float){ state->window->width / 2.0f, state->window->height / 2.0f };

    state->cameraEntity.position.x = state->camera.position.x;
    state->cameraEntity.position.y = state->camera.position.y;
    state->cameraEntity.scale      = 1.0f / state->camera.zoom;

    UI_Initialize(&state->uiState, &state->renderStack, state->window, &state->camera);
}
