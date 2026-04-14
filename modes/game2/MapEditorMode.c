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

enum TileType
{
    TILE_TYPE_EMPTY         = 0,
    TILE_TYPE_SOLID         = 1,
    TILE_TYPE_JUMP_PLATFORM = 2,
    TILE_TYPE_PLAYER_SPAWN  = 3,
    TILE_TYPE_ENEMY_SPAWN   = 4,
};

struct Tile
{
    Vector2Int position;
    uint16_t   textureId;
    TileType   type;
};

struct MapData
{
    Tile     tiles[1024];
    uint16_t tileCount = 0;
};

struct EditorData
{
    int32_t  selectedTile = -1;
    bool     overMap      = false;
    TileType tileType     = TILE_TYPE_EMPTY;
    MapData  mapData;
} data;

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


    data.overMap       = true;
    Vector2Float uiPos = Utils_ScreenToWorld2D({ 0.0f, 0.0f }, *Window_GetCamera());

    for (uint16_t i = 0; i < (16 * 24); i++)
    {
        uint16_t posX = i % 16;
        uint16_t posY = i / 16;
        Button   button;
        button.scale = 1.85f;
        button.position =
            (Vector2Float){ uiPos.x + posX * 16.0f * button.scale, uiPos.y + posY * 16.0f * button.scale };
        button.onTexture  = &textures[i];
        button.offTexture = &textures[i];
        button.bounds     = (Rectangle){ button.position.x, button.position.y, (float)textures[i].size.x * button.scale,
                                         (float)textures[i].size.y * button.scale };
        if (UI_TextureButton(&button))
        {
            if (Input_IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                if (data.selectedTile == i)
                {
                    data.selectedTile = -1;
                }
                else
                {
                    data.selectedTile = i;
                }
            }
            data.overMap = false;
        }
        if (data.selectedTile == i)
        {
            sprites[spriteCount - 1].tint = (Color){ 255, 0, 255, 255 };
            button.isPressed              = true;
        }
    }


    if (data.overMap)
    {
        Vector2Float mouseWorldPos =
            Utils_ScreenToWorld2D({ float(Input_GetMouseX()), float(Input_GetMouseY()) }, *Window_GetCamera());
        if (data.selectedTile != -1)
        {
            Vector3Int gridPos = Utils_WorldToGrid(mouseWorldPos, 32);
            Sprite*    sprite  = &sprites[spriteCount++];
            Sprite_Initialize(sprite);
            sprite->scale          = 2.0f;
            sprite->position.x     = gridPos.x * 32.0f;
            sprite->position.y     = gridPos.y * 32.0f;
            sprite->currentTexture = &textures[data.selectedTile];
            if (Input_IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                if (data.mapData.tileCount < 1024)
                {
                    Tile* tile       = &data.mapData.tiles[data.mapData.tileCount++];
                    tile->position.x = gridPos.x;
                    tile->position.y = gridPos.y;
                    tile->textureId  = data.selectedTile;
                    tile->type       = data.tileType;
                }
            }
        }
    }
    for (uint16_t i = 0; i < data.mapData.tileCount; i++)
    {
        Tile*   tile   = &data.mapData.tiles[i];
        Sprite* sprite = &sprites[spriteCount++];
        Sprite_Initialize(sprite);
        sprite->scale          = 2.0f;
        sprite->position.x     = tile->position.x * 32.0f;
        sprite->position.y     = tile->position.y * 32.0f;
        sprite->currentTexture = &textures[tile->textureId];
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
