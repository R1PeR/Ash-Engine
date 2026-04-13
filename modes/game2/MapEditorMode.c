#include "MapEditorMode.h"

#include "ashes/ash_components.h"
#include "ashes/ash_context.h"
#include "ashes/ash_debug.h"
#include "ashes/ash_io.h"
#include "ashes/ash_misc.h"
#include "utils/UI.h"

Mode mapEditorMode = MODE_FROM_CLASSNAME(MapEditorMode);

static Sprite      sprites[2048];
static uint16_t    spriteCount = 0;
static TextureData textures[512];
static TextureData texture;

static Entity2D cameraEntity;

void MapEditorMode_OnStart()
{
    texture = Texture_LoadTexture("resources/sprites/tileset.png");
    LOG_INF("Loaded texture: %s, width: %d, height: %d", "resources/sprites/player.png", texture.size.x,
            texture.size.y);
    if (!Texture_CreateTextureAtlas(texture, 24, 16, textures))
    {
        LOG_ERR("Failed to create texture atlas");
    }
    for (uint32_t i = 0; i < 128; i++)
    {
        Sprite_Initialize(&sprites[i]);
    }
    Entity2D_Initialize(&cameraEntity);
    UI_Initialize(&cameraEntity, sprites, (size_t*)&spriteCount, 2048);
}

void MapEditorMode_OnPause()
{
}

void MapEditorMode_Update()
{
    spriteCount = 0;
    DeltaTime_Update();

    cameraEntity.position.x = Window_GetCamera()->target.x;
    cameraEntity.position.y = Window_GetCamera()->target.y;
    cameraEntity.scale      = Window_GetCamera()->zoom;

    Vector2Float uiPos = Utils_ScreenToWorld2D({ 0.0f, 0.0f }, *Window_GetCamera());

    for (uint16_t i = 0; i < (16 * 24); i++)
    {
        uint16_t posX = i % 16;
        uint16_t posY = i / 16;
        Button button;
        button.scale      = 1.85f;
        button.position   = (Vector2Float){ uiPos.x + posX * 16.0f * button.scale, uiPos.y + posY * 16.0f * button.scale };
        button.onTexture  = &textures[i];
        button.offTexture = &textures[i];
        button.bounds     = (Rectangle){ button.position.x, button.position.y, (float)textures[i].size.x * button.scale,
                                         (float)textures[i].size.y * button.scale };
        if (UI_TextureButton(&button))
        {

        }
    }
    for (uint16_t i = 0; i < spriteCount; i++)
    {
        Sprite_Draw(&sprites[i]);
    }
}

void MapEditorMode_OnStop()
{
}

void MapEditorMode_OnResume()
{
}
