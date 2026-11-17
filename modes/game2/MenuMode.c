#include "MenuMode.h"

#include "MainMode.h"
#include "MapEditorMode.h"
#include "ashes/ash_components.h"
#include "ashes/ash_context.h"
#include "ashes/ash_debug.h"
#include "ashes/ash_io.h"
#include "ashes/ash_misc.h"
#include "utils/ArenaDebugFrame.h"

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
    state->camera.offset   = (Vector2Float){ state->window->width/2.0f, state->window->height/2.0f };

    state->cameraEntity.position.x = state->camera.position.x;
    state->cameraEntity.position.y = state->camera.position.y;
    state->cameraEntity.scale      = 1.0f / state->camera.zoom;

    UI_Initialize(&state->uiState, state->renderStackItems, &state->renderStackItemCount, STACKITEM_MAX, state->window,
                  &state->camera);
    UI_SetParentEntity(&state->uiState, &state->cameraEntity);
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

    state->renderStackItems[state->renderStackItemCount].type              = RENDERCOMMAND_CLEAR_SCREEN;
    state->renderStackItems[state->renderStackItemCount].clearScreen.color = (Color){ 0, 0, 0, 255 };
    state->renderStackItemCount++;


    state->renderStackItems[state->renderStackItemCount].type            = RENDERCOMMAND_BEGIN_CAMERA2D;
    state->renderStackItems[state->renderStackItemCount].camera.camera2D = &state->camera;
    state->renderStackItemCount++;

    state->renderStackItems[state->renderStackItemCount].type = RENDERCOMMAND_BEGIN2D;
    state->renderStackItemCount++;

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

    ArenaDebugFrame_DisplayFrame(&state->uiState, state->fontTextures, state->arena);


    state->renderStackItems[state->renderStackItemCount].type = RENDERCOMMAND_END_CAMERA2D;
    state->renderStackItemCount++;
    state->renderStackItems[state->renderStackItemCount].type = RENDERCOMMAND_END2D;
    state->renderStackItemCount++;
}

void MenuMode_Draw(Context* context, void* data)
{
    (void)context;
    MenuMode* state              = (MenuMode*)data;
    state->renderStack.stack     = state->renderStackItems;
    state->renderStack.stackSize = state->renderStackItemCount;
    RenderStack_Draw(&state->renderStack);
}

void MenuMode_OnStop(Context* context, void* data)
{
    (void)context;
    (void)data;
}

void MenuMode_OnResume(Context* context, void* data)
{
    (void)context;
    MenuMode* state        = (MenuMode*)data;
    state->camera.zoom     = 1.0f;
    state->camera.position = (Vector2Float){ 0.0f, 0.0f };
    state->camera.rotation = 0.0f;
    state->camera.offset   = (Vector2Float){ state->window->width/2.0f, state->window->height/2.0f };

    state->cameraEntity.position.x = state->camera.position.x;
    state->cameraEntity.position.y = state->camera.position.y;
    state->cameraEntity.scale      = 1.0f / state->camera.zoom;

    UI_Initialize(&state->uiState, state->renderStackItems, (size_t*)&state->renderStackItemCount, STACKITEM_MAX,
                  state->window, &state->camera);
}
