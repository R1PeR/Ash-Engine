#include "MainMode.h"

#include "engine/components/Collider2D.h"
#include "engine/components/Entity2D.h"
#include "engine/components/Sprite.h"
#include "engine/io/Window.h"
#include "game/GameWindow.h"
#include "game/objects/Player.h"
#include "libs/engine/components/AsciiWindow.h"
#include "libs/engine/components/Audio.h"
#include "libs/engine/components/AudioPlayer.h"
#include "libs/engine/components/Texture.h"
#include "libs/engine/io/Input.h"
#include "libs/engine/misc/DeltaTime.h"
#include "libs/engine/misc/Logger.h"
#include "libs/engine/misc/Stopwatch.h"
#include "libs/engine/misc/Utils.h"
#include "raylib.h"
#include "utils/SpriteDefines.h"

#define CHUNK_SIZE        16
#define CHUNK_MAX_OBJECTS 256
#define TEXTURE_SIZE      32
#define SPRITE_MAX_COUNT  256

Mode mainMode = MODE_FROM_CLASSNAME(MainMode);

enum Type : uint16_t
{
    TILE = 0,
    PLAYER,
    ENEMY,
    PROJECTILE,
    EFFECT,
};

enum Tiles : uint32_t
{
    TILE_EMPTY = 0,
    TILE_GRASS = 1,
    TILE_SAND  = 2,
};

struct Object
{
    uint8_t     type;
    uint8_t     layer;
    Vector2Int8 position;
};

struct Chunk
{
    Vector3Int8 chunkPosition;
    Object      objects[CHUNK_MAX_OBJECTS];
    uint16_t    objectCount;
};

struct GameData
{
    Sprite   sprites[SPRITE_MAX_COUNT];
    Chunk    chunks[CHUNK_SIZE * CHUNK_SIZE];
    uint16_t chunkCount;
} gamedata;

struct TextureButton
{
    Collider2D collider;
    Sprite     sprite;
    uint8_t    textureId;
    bool       isSelected;
};

struct DebugData
{
    TextureButton textureButtons[4];
    Entity2D      parent;
} debugData;

void InitDebug()
{
    Entity2D_Initialize(&debugData.parent);
    for (int i = 0; i < 1; i++)
    {
        Collider2D_Initialize(&debugData.textureButtons[i].collider);
        Sprite_Initialize(&debugData.textureButtons[i].sprite);
        debugData.textureButtons[i].collider.parent = &debugData.parent;
        debugData.textureButtons[i].sprite.parent   = &debugData.parent;
    }
}

void DrawDebug()
{
    debugData.parent.position = Window_GetCamera()->target;
    debugData.parent.scale    = 1.0f / Window_GetCamera()->zoom;

    if (Input_IsKeyDown(INPUT_KEYCODE_W) || Input_IsKeyDown(INPUT_KEYCODE_S) || Input_IsKeyDown(INPUT_KEYCODE_A)
        || Input_IsKeyDown(INPUT_KEYCODE_D) || Input_IsKeyDown(INPUT_KEYCODE_Q) || Input_IsKeyDown(INPUT_KEYCODE_E))
    {
        int8_t y = (Input_IsKeyDown(INPUT_KEYCODE_W) - Input_IsKeyDown(INPUT_KEYCODE_S));
        int8_t x = (Input_IsKeyDown(INPUT_KEYCODE_A) - Input_IsKeyDown(INPUT_KEYCODE_D));
        int8_t z = (Input_IsKeyDown(INPUT_KEYCODE_Q) - Input_IsKeyDown(INPUT_KEYCODE_E));
        Window_GetCamera()->target.x -= x / Window_GetCamera()->zoom * 2;
        Window_GetCamera()->target.y -= y / Window_GetCamera()->zoom * 2;
        Window_GetCamera()->zoom += -z * 0.05f;
        if (Window_GetCamera()->zoom > 1.0f)
        {
            Window_GetCamera()->zoom = 1.0f;
        }
        else if (Window_GetCamera()->zoom < 0.1f)
        {
            Window_GetCamera()->zoom = 0.1f;
        }
    }
    for (int y = -5; y < 5; y++)
    {
        for (int x = -5; x < 5; x++)
        {
            DrawRectangleLines(CHUNK_SIZE * TEXTURE_SIZE * x, CHUNK_SIZE * TEXTURE_SIZE * y, CHUNK_SIZE * TEXTURE_SIZE,
                               CHUNK_SIZE * TEXTURE_SIZE, RED);
        }
    }
}

void MainMode_OnStart()
{
    Texture_LoadTextureSheet("resources/sprites/Anikki_square_8x8.png", 8, 8, 256);
    gamedata.chunkCount = 0;
    // chunk debug
}

void MainMode_OnPause()
{
}

void MainMode_Update()
{
    DrawDebug();
}

void MainMode_OnStop()
{
    Texture_UnloadTextures();
    Audio_UnloadAudios();
}

void MainMode_OnResume()
{
}
