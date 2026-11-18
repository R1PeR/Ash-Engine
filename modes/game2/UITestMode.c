#include "UITestMode.h"

#include "ashes/ash_components.h"
#include "ashes/ash_context.h"
#include "ashes/ash_debug.h"
#include "ashes/ash_io.h"
#include "ashes/ash_misc.h"
#include "ashes/ash_platform.h"

#include <stdio.h>
#include <string.h>

static void UITestExampleGUI(Context* context, UITestMode* state)
{
    (void)context;
    UI_Begin(&state->uiState, (Vector4Float){ -640.0f, -360.0f, 1280.0f, 720.0f });
    {
        UI_Frame(&state->uiState);
        {
            UI_Layout(&state->uiState, LayoutVertical);
            UI_Center(&state->uiState, CenterBoth);
            UI_Text(&state->uiState, "Hello UI_V2!", 3.0f, state->fontTextures);
            UI_Text(&state->uiState, "Immediate Mode UI", 2.0f, state->fontTextures);
        }
        UI_Frame(&state->uiState);
        {
            UI_Layout(&state->uiState, LayoutVertical);
            UI_Center(&state->uiState, CenterBoth);
            UI_Text(&state->uiState, "Button Demo", 1.5f, state->fontTextures);
            if (UI_Button(&state->uiState, "Click Me", 2.0f, state->fontTextures))
                state->btnCount++;
            snprintf(state->btnLabel, sizeof(state->btnLabel), "Clicked: %d", state->btnCount);
            UI_Text(&state->uiState, state->btnLabel, 1.5f, state->fontTextures);
        }
        UI_Frame(&state->uiState);
        {
            UI_Layout(&state->uiState, LayoutVertical);
            UI_Center(&state->uiState, CenterBoth);
            UI_Text(&state->uiState, "Slider Demo", 1.5f, state->fontTextures);
            state->sliderVal = UI_Slider(&state->uiState, state->sliderVal, 0.0f, 1.0f);
            {
                char buf[32];
                snprintf(buf, sizeof(buf), "Value: %.2f", state->sliderVal);
                UI_Text(&state->uiState, buf, 1.5f, state->fontTextures);
            }
        }
        UI_Frame(&state->uiState);
        {
            UI_Layout(&state->uiState, LayoutVertical);
            UI_Center(&state->uiState, CenterBoth);
            UI_Text(&state->uiState, "List Demo", 1.5f, state->fontTextures);
            state->listChoice = UI_List(&state->uiState, state->listItems, state->listItemCount, 1.2f, 18.0f,
                                        state->fontTextures, true);
            {
                char buf[64];
                if (state->listChoice >= 0)
                    snprintf(buf, sizeof(buf), "Selected: %s", state->listItems[state->listChoice]);
                else
                    snprintf(buf, sizeof(buf), "Nothing selected");
                UI_Text(&state->uiState, buf, 1.2f, state->fontTextures);
            }
        }
    }
    UI_End(&state->uiState);
}

void UITestMode_OnStart(Context* context, void* data)
{
    (void)context;
    UITestMode* state    = (UITestMode*)data;
    state->tileAtlasBase = Texture_LoadTexture("resources/sprites/tileset.png", NULL);
    LOG_INF("UITestMode: tile atlas %dx%d", state->tileAtlasBase.size.x, state->tileAtlasBase.size.y);
    if (!Texture_CreateTextureAtlas(state->tileAtlasBase, TILESET_ATLAS_COLS, TILESET_ATLAS_ROWS, state->tileTextures))
        LOG_ERR("MapEditor: failed to create tile atlas");

    state->fontAtlasBase = Texture_LoadTexture("resources/sprites/Anikki_square_8x8.png", NULL);
    if (!Texture_CreateTextureAtlas(state->fontAtlasBase, FONT_ATLAS_COLS, FONT_ATLAS_ROWS, state->fontTextures))
        LOG_ERR("MapEditor: failed to create font atlas");

    Entity2D_Initialize(&state->cameraEntity);

    state->camera.zoom     = 1.0f;
    state->camera.rotation = 0.0f;
    state->camera.offset   = (Vector2Float){ 0.0f, 0.0f };
    state->camera.position = (Vector2Float){ 0.0f, 0.0f };

    state->cameraEntity.position.x = state->camera.position.x;
    state->cameraEntity.position.y = state->camera.position.y;
    state->cameraEntity.scale      = 1.0f / state->camera.zoom;

    UI_Initialize(&state->uiState, &state->renderStack, state->window, &state->camera);

    Sprite_Initialize(&state->testSprite);
    state->testSprite.currentTexture = &state->tileTextures[0];
    state->testSprite.scale          = 4.0f;
}

void UITestMode_OnPause(Context* context, void* data)
{
    (void)context;
    (void)data;
}

void UITestMode_Update(Context* context, void* data)
{
    (void)context;
    UITestMode* state              = (UITestMode*)data;
    state->cameraEntity.position.x = state->camera.position.x;
    state->cameraEntity.position.y = state->camera.position.y;
    state->cameraEntity.scale      = 1.0f / state->camera.zoom;

    DeltaTime_Update(&state->deltaTime);

    RenderStack_Clear(&state->renderStack);
    RenderStack_PushClearScreen(&state->renderStack, (Color){ 0, 0, 0, 255 });
    RenderStack_PushCamera2d(&state->renderStack, &state->camera);
    RenderStack_PushMode2d(&state->renderStack);

    UITestExampleGUI(context, state);

    RenderStack_PopMode2d(&state->renderStack);
    RenderStack_PopCamera2d(&state->renderStack);
}

void UITestMode_Draw(Context* context, void* data)
{
    (void)context;
    UITestMode* state = (UITestMode*)data;
    RenderStack_Draw(&state->renderStack);
}

void UITestMode_OnStop(Context* context, void* data)
{
    (void)context;
    (void)data;
}

void UITestMode_OnResume(Context* context, void* data)
{
    (void)context;
    (void)data;
}
