#include "UITestMode.h"

#include "ashes/ash_components.h"
#include "ashes/ash_context.h"
#include "ashes/ash_debug.h"
#include "ashes/ash_io.h"
#include "ashes/ash_misc.h"
#include "utils/UI.h"

#include <raylib.h>
#include <stdio.h>
#include <string.h>

#define DRAWABLE_MAX 4096

#define MAP_TILESET_COLS  24
#define MAP_TILESET_ROWS  16
#define MAP_TILESET_COUNT (MAP_TILESET_COLS * MAP_TILESET_ROWS)

#define TILESET_ATLAS_COLS MAP_TILESET_COLS
#define TILESET_ATLAS_ROWS MAP_TILESET_ROWS
#define TILESET_COUNT      MAP_TILESET_COUNT

#define FONT_ATLAS_COLS  16
#define FONT_ATLAS_ROWS  16
#define FONT_GLYPH_COUNT (FONT_ATLAS_COLS * FONT_ATLAS_ROWS)

Mode uiTestMode = MODE_FROM_CLASSNAME(UITestMode);

static Drawable drawables[DRAWABLE_MAX];
static size_t  drawableCount = 0;

static TextureData tileTextures[TILESET_COUNT];
static TextureData tileAtlasBase;

static TextureData fontTextures[FONT_GLYPH_COUNT];
static TextureData fontAtlasBase;

static Entity2D  cameraEntity;
static Sprite    testSprite;

static float    sliderVal = 0.5f;
static int      listChoice = -1;
static int      btnCount = 0;
static char     btnLabel[64];
static const char* listItems[] = { "Apples", "Bananas", "Cherries", "Dates", "Elderberries", "Figs", "Grapes", "Honeydew", "Indian Fig", "Jackfruit", "Kiwi", "Lemon", "Mango", "Nectarine", "Orange", "Papaya" };
static int      listItemCount = sizeof(listItems) / sizeof(listItems[0]);

static void UITestExampleGUI()
{
    UI_Begin((Vector4Float){ -640.0f, -360.0f, 1280.0f, 720.0f });
    {
        UI_Frame();
        {
            UI_Layout(LayoutVertical);
            UI_Center(CenterBoth);
            UI_Text("Hello UI_V2!", 3.0f, fontTextures);
            UI_Text("Immediate Mode UI", 2.0f, fontTextures);
        }
        UI_Frame();
        {
            UI_Layout(LayoutVertical);
            UI_Center(CenterBoth);
            UI_Text("Button Demo", 1.5f, fontTextures);
            if (UI_Button("Click Me", 2.0f, fontTextures))
                btnCount++;
            snprintf(btnLabel, sizeof(btnLabel), "Clicked: %d", btnCount);
            UI_Text(btnLabel, 1.5f, fontTextures);
        }
        UI_Frame();
        {
            UI_Layout(LayoutVertical);
            UI_Center(CenterBoth);
            UI_Text("Slider Demo", 1.5f, fontTextures);
            sliderVal = UI_Slider(sliderVal, 0.0f, 1.0f);
            {
                char buf[32];
                snprintf(buf, sizeof(buf), "Value: %.2f", sliderVal);
                UI_Text(buf, 1.5f, fontTextures);
            }
        }
        UI_Frame();
        {
            UI_Layout(LayoutVertical);
            UI_Center(CenterBoth);
            UI_Text("List Demo", 1.5f, fontTextures);
            listChoice = UI_List(listItems, listItemCount, 1.2f, 18.0f, fontTextures, true);
            {
                char buf[64];
                if (listChoice >= 0)
                    snprintf(buf, sizeof(buf), "Selected: %s", listItems[listChoice]);
                else
                    snprintf(buf, sizeof(buf), "Nothing selected");
                UI_Text(buf, 1.2f, fontTextures);
            }
        }
    }
    UI_End();
}

void UITestMode_OnStart()
{
    tileAtlasBase = Texture_LoadTexture("resources/sprites/tileset.png");
    LOG_INF("UITestMode: tile atlas %dx%d", tileAtlasBase.size.x, tileAtlasBase.size.y);
    if (!Texture_CreateTextureAtlas(tileAtlasBase, TILESET_ATLAS_COLS, TILESET_ATLAS_ROWS, tileTextures))
        LOG_ERR("MapEditor: failed to create tile atlas");

    fontAtlasBase = Texture_LoadTexture("resources/sprites/Anikki_square_8x8.png");
    if (!Texture_CreateTextureAtlas(fontAtlasBase, FONT_ATLAS_COLS, FONT_ATLAS_ROWS, fontTextures))
        LOG_ERR("MapEditor: failed to create font atlas");

    Entity2D_Initialize(&cameraEntity);

    Camera2D* camera = Window_GetCamera();
    camera->zoom     = 1.0f;
    camera->target   = (Vector2){ 0.0f, 0.0f };

    cameraEntity.position.x = camera->target.x;
    cameraEntity.position.y = camera->target.y;
    cameraEntity.scale      = 1.0f / camera->zoom;

    UI_Initialize(drawables, (size_t*)&drawableCount, DRAWABLE_MAX);

    Sprite_Initialize(&testSprite);
    testSprite.currentTexture = &tileTextures[0];
    testSprite.scale          = 4.0f;
}

void UITestMode_OnPause()
{
}

void UITestMode_Update()
{
    cameraEntity.position.x = Window_GetCamera()->target.x;
    cameraEntity.position.y = Window_GetCamera()->target.y;
    cameraEntity.scale      = 1.0f / Window_GetCamera()->zoom;

    drawableCount = 0;
    DeltaTime_Update();

    UITestExampleGUI();
    for (size_t i = 0; i < drawableCount; i++)
        Drawable_Draw(&drawables[i]);
}

void UITestMode_OnStop()
{
}

void UITestMode_OnResume()
{
}
