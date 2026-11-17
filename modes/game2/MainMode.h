#ifndef LIBS_ENGINE_MAINMODE_H
#define LIBS_ENGINE_MAINMODE_H
#include "ashes/ash_components.h"
#include "ashes/ash_context.h"
#include "modes/game2/MapEditorMode.h"
#include "utils/UI.h"

#include <stdint.h>

#define TILESET_ATLAS_COLS     MAP_TILESET_COLS
#define TILESET_ATLAS_ROWS     MAP_TILESET_ROWS
#define TILESET_COUNT          MAP_TILESET_COUNT
#define FONT_ATLAS_COLS        16
#define FONT_ATLAS_ROWS        16
#define FONT_GLYPH_COUNT       (FONT_ATLAS_COLS * FONT_ATLAS_ROWS)
#define RENDERSTACKITEM_MAX           4096
#define DIRECTION_SPEED_GROUND 1200.0f
#define DIRECTION_SPEED_AIR    600.0f
#define FRICTION_GROUND        600.0f
#define FRICTION_AIR           100.0f
#define FRICTION_WALL          600.0f
#define GRAVITY                980.0f
#define PLAYER_MAX_SPEED       200.0f   // maximum speed from controls
#define PLAYER_MAX_VELOCITY    1000.0f  // maximum speed from physics
#define PLAYER_JUMP_FORCE      400.0f

struct Player
{
    Entity2D       entity;
    Vector2Float   velocity;
    Collider2D     collider;
    uint8_t        collisionFlags;
    bool           onGround;
    int8_t         onWall;
    Vector2Float   velocityBeforeCollision;
    Stopwatch      jumpCoyoteTime;
    Stopwatch      wallCoyoteTime;
    AnimatedSprite sprite;
};

struct Platform
{
    Entity2D   entity;
    Collider2D collider;
    Sprite     sprite;
};

struct Map
{
    Platform platforms[128];
    uint8_t  platformCount;
};

struct GameData
{
    float         dt;
    Player        player;
    Map           map;
    AnimationData playerIdleAnimation;
    AnimationData playerWalkAnimation;
    AnimationData playerAirIdleAnimation;
    AnimationData playerAirMoveAnimation;
    AnimationData playerWallSlideAnimation;
};

struct MainMode
{
    TextureData tileTextures[TILESET_COUNT];
    TextureData tileAtlasBase;

    TextureData fontTextures[FONT_GLYPH_COUNT];
    TextureData fontAtlasBase;

    RenderStack     renderStack;
    RenderStackItem renderStackItems[RENDERSTACKITEM_MAX];
    size_t          renderStackItemCount = 0;

    TextureData editorTileTextures[MAP_TILESET_COUNT];
    TextureData editorTileAtlasBase;
    bool        editorTilesLoaded = false;

    Entity2D cameraEntity;
    UI_State uiState;

    GameData  gameData;
    Window*   window;
    DeltaTime deltaTime;
    Arena*    arena;
    Camera2D  camera;
};

void MainMode_OnStart(Context* context, void* data);
void MainMode_OnPause(Context* context, void* data);
void MainMode_Update(Context* context, void* data);
void MainMode_Draw(Context* context, void* data);
void MainMode_OnStop(Context* context, void* data);
void MainMode_OnResume(Context* context, void* data);

#endif
