#include "MenuMode.h"

#include "MainMode.h"
#include "MapEditorMode.h"
#include "ashes/ash_components.h"
#include "ashes/ash_context.h"
#include "ashes/ash_debug.h"
#include "ashes/ash_io.h"
#include "ashes/ash_misc.h"
#include "utils/UI.h"

#include <raylib.h>
#include <stdio.h>

#define DRAWABLE_MAX    4096
#define FONT_ATLAS_COLS 16
#define FONT_ATLAS_ROWS 16
#define FONT_GLYPH_COUNT (FONT_ATLAS_COLS * FONT_ATLAS_ROWS)

Mode menuMode = MODE_FROM_CLASSNAME(MenuMode);

static Drawable drawables[DRAWABLE_MAX];
static size_t  drawableCount = 0;

static TextureData fontTextures[FONT_GLYPH_COUNT];
static TextureData fontAtlasBase;

static Entity2D cameraEntity;

void MenuMode_OnStart()
{
    fontAtlasBase = Texture_LoadTexture("resources/sprites/Anikki_square_8x8.png");
    if (!Texture_CreateTextureAtlas(fontAtlasBase, FONT_ATLAS_COLS, FONT_ATLAS_ROWS, fontTextures))
        LOG_ERR("MenuMode: failed to create font atlas");

    Entity2D_Initialize(&cameraEntity);

    Camera2D* camera = Window_GetCamera();
    camera->zoom     = 1.0f;
    camera->target   = (Vector2){ 0.0f, 0.0f };

    cameraEntity.position.x = camera->target.x;
    cameraEntity.position.y = camera->target.y;
    cameraEntity.scale      = 1.0f / camera->zoom;

    UI_Initialize(drawables, (size_t*)&drawableCount, DRAWABLE_MAX);
}

void MenuMode_OnPause()
{
}

void MenuMode_Update()
{
    cameraEntity.position.x = Window_GetCamera()->target.x;
    cameraEntity.position.y = Window_GetCamera()->target.y;
    cameraEntity.scale      = 1.0f / Window_GetCamera()->zoom;

    drawableCount = 0;
    DeltaTime_Update();

    UI_Begin((Vector4Float){ -640.0f, -360.0f, 1280.0f, 720.0f });
    {
        UI_Frame();
        {
            UI_Layout(LayoutVertical);
            UI_Center(CenterBoth);
            UI_Text("DON'T YOU DARE GO HOLLOW", 3.0f, fontTextures);
        }
        UI_Frame();
        {
            UI_Layout(LayoutVertical);
            UI_Center(CenterBoth);
            if (UI_Button("Start Game", 2.0f, fontTextures))
                Context_SetMode(&mainMode);
            if (UI_Button("Map Editor", 2.0f, fontTextures))
                Context_SetMode(&mapEditorMode);
            if (UI_Button("Exit", 2.0f, fontTextures))
                Context_FinishMode();
        }
    }
    UI_End();

    for (size_t i = 0; i < drawableCount; i++)
        Drawable_Draw(&drawables[i]);
}

void MenuMode_OnStop()
{
}

void MenuMode_OnResume()
{
}
