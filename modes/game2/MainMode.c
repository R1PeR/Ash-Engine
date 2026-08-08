#include "MainMode.h"

#include "MapEditorMode.h"
#include "ashes/ash_components.h"
#include "ashes/ash_context.h"
#include "ashes/ash_debug.h"
#include "ashes/ash_io.h"
#include "ashes/ash_misc.h"
#include "utils/UI.h"

#include <stdio.h>
#include <string.h>

#define TILESET_ATLAS_COLS     MAP_TILESET_COLS
#define TILESET_ATLAS_ROWS     MAP_TILESET_ROWS
#define TILESET_COUNT          MAP_TILESET_COUNT
#define FONT_ATLAS_COLS        16
#define FONT_ATLAS_ROWS        16
#define FONT_GLYPH_COUNT       (FONT_ATLAS_COLS * FONT_ATLAS_ROWS)
#define DRAWABLE_MAX           4096
#define DIRECTION_SPEED_GROUND 1200.0f
#define DIRECTION_SPEED_AIR    600.0f
#define FRICTION_GROUND        600.0f
#define FRICTION_AIR           100.0f
#define FRICTION_WALL          600.0f
#define GRAVITY                980.0f
#define PLAYER_MAX_SPEED       200.0f   // maximum speed from controls
#define PLAYER_MAX_VELOCITY    1000.0f  // maximum speed from physics
#define PLAYER_JUMP_FORCE      400.0f

Mode mainMode = MODE_FROM_CLASSNAME(MainMode);

struct Player
{
    Entity2D     entity;
    Vector2Float velocity;
    Collider2D   collider;
    uint8_t      collisionFlags;
    bool         onGround;
    int8_t       onWall;
    Vector2Float velocityBeforeCollision;
    Stopwatch    jumpCoyoteTime;
    Stopwatch    wallCoyoteTime;
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
    float  dt;
    Player player;
    Map    map;

} gameData;

static TextureData tileTextures[TILESET_COUNT];
static TextureData tileAtlasBase;

static TextureData fontTextures[FONT_GLYPH_COUNT];
static TextureData fontAtlasBase;

static Drawable drawables[DRAWABLE_MAX];
static size_t   drawableCount = 0;

static TextureData editorTileTextures[MAP_TILESET_COUNT];
static TextureData editorTileAtlasBase;
static bool        editorTilesLoaded = false;

static Entity2D cameraEntity;

void UpdateDebug()
{
    char fps[32];
    snprintf(fps, sizeof(fps), "FPS: %d", (int)(1.0f / gameData.dt));
    char deltaTime[32];
    snprintf(deltaTime, sizeof(deltaTime), "Delta Time: %.4f", gameData.dt);
    char playerPos[32];
    snprintf(playerPos, sizeof(playerPos), "Player Pos: (%.2f, %.2f)", gameData.player.entity.position.x,
             gameData.player.entity.position.y);
    char playerVel[32];
    snprintf(playerVel, sizeof(playerVel), "Player Vel: (%.2f, %.2f)", gameData.player.velocity.x,
             gameData.player.velocity.y);
    char playerOnGround[32];
    snprintf(playerOnGround, sizeof(playerOnGround), "Player onGround: %s",
             gameData.player.onGround ? "true" : "false");
    char playerOnWall[32];
    snprintf(playerOnWall, sizeof(playerOnWall), "Player onWall: %s", (gameData.player.onWall != 0) ? "true" : "false");
    char playerJumpCoyoteTime[32];
    snprintf(playerJumpCoyoteTime, sizeof(playerJumpCoyoteTime), "Player jumpCoyoteTime: %s",
             Stopwatch_IsRunning(&gameData.player.jumpCoyoteTime) ? "true" : "false");
    char playerWallCoyoteTime[32];
    snprintf(playerWallCoyoteTime, sizeof(playerWallCoyoteTime), "Player wallCoyoteTime: %s",
             Stopwatch_IsRunning(&gameData.player.wallCoyoteTime) ? "true" : "false");
    char cameraPos[32];
    snprintf(cameraPos, sizeof(cameraPos), "Camera Pos: (%.2f, %.2f)", Window_GetCamera()->target.x,
             Window_GetCamera()->target.y);


    UI_Begin(UI_GetBounds(AnchorTopLeft, { 0, 0, 0.4, 0.3 }));
    UI_Frame();
    {
        UI_Layout(LayoutVertical);
        // UI_Center(Center);
        UI_Padding(UI_GetSize({ 0.01, 0.01, 0.01, 0.01 }));
        UI_Text("DEBUG INFO", 2.0f, fontTextures);
        UI_Text(fps, 1.0f, fontTextures);
        UI_Text(deltaTime, 1.0f, fontTextures);
        UI_Text(playerPos, 1.0f, fontTextures);
        UI_Text(playerVel, 1.0f, fontTextures);
        UI_Text(playerOnGround, 1.0f, fontTextures);
        UI_Text(playerOnWall, 1.0f, fontTextures);
        UI_Text(playerJumpCoyoteTime, 1.0f, fontTextures);
        UI_Text(playerWallCoyoteTime, 1.0f, fontTextures);
        UI_Text(cameraPos, 1.0f, fontTextures);
    }
    UI_End();
}

void UpdateEditorTiles()
{
    if (!g_editorTestMapData.isValid || !editorTilesLoaded)
        return;
    for (int l = 0; l < MAP_MAX_LAYERS; l++)
    {
        TileLayer* layer = &g_editorTestMapData.mapData.layers[l];
        for (int i = 0; i < (int)layer->tileCount && drawableCount < DRAWABLE_MAX; i++)
        {
            Tile* tile = &layer->tiles[i];
            if (tile->textureId >= MAP_TILESET_COUNT)
                continue;
            Sprite sprite;
            Sprite_Initialize(&sprite);
            sprite.currentTexture           = &editorTileTextures[tile->textureId];
            sprite.position.x               = (float)(tile->position.x * TILE_SIZE);
            sprite.position.y               = (float)(tile->position.y * TILE_SIZE);
            sprite.scale                    = 2.0f;
            drawables[drawableCount].sprite = sprite;
            drawables[drawableCount].type   = DRAWABLE_SPRITE;
            drawableCount++;
        }
    }
}

void UpdateGame()
{
    // get input
    int  directionX = { -Input_IsKeyDown(KEY_A) + Input_IsKeyDown(KEY_D) };
    int  directionY = { -Input_IsKeyDown(KEY_S) + Input_IsKeyDown(KEY_W) };
    bool jump       = Input_IsKeyPressed(KEY_SPACE);

    // update player state
    if (gameData.player.onGround)
    {
        Stopwatch_Start(&gameData.player.jumpCoyoteTime, 100);
        Stopwatch_Stop(&gameData.player.wallCoyoteTime);
        gameData.player.onWall = 0;
        // Apply ground friction
        if (gameData.player.velocity.x > 0.0f)
        {
            float deltaFriction = FRICTION_GROUND * gameData.dt;
            if (gameData.player.velocity.x - deltaFriction < 0.0f)
            {
                gameData.player.velocity.x = 0.0f;
            }
            else
            {
                gameData.player.velocity.x -= deltaFriction;
            }
        }
        if (gameData.player.velocity.x < 0.0f)
        {
            float deltaFriction = FRICTION_GROUND * gameData.dt;
            if (gameData.player.velocity.x + deltaFriction > 0.0f)
            {
                gameData.player.velocity.x = 0.0f;
            }
            else
            {
                gameData.player.velocity.x += deltaFriction;
            }
        }
        // Apply ground movement
        if (gameData.player.velocity.x < PLAYER_MAX_SPEED && gameData.player.velocity.x > -PLAYER_MAX_SPEED)
        {
            gameData.player.velocity.x += (directionX * DIRECTION_SPEED_GROUND * gameData.dt);
        }
        gameData.player.velocity.y += GRAVITY * gameData.dt;
    }
    else if (gameData.player.onWall != 0)
    {
        Stopwatch_Stop(&gameData.player.jumpCoyoteTime);
        // Apply gravity
        gameData.player.velocity.y += GRAVITY * gameData.dt;
        // Apply wall friction
        if (gameData.player.velocity.y > 0.0f)
        {
            float deltaFriction = FRICTION_WALL * gameData.dt;
            gameData.player.velocity.y -= deltaFriction;
        }
        if (gameData.player.velocity.y < 0.0f)
        {
            float deltaFriction = FRICTION_WALL * gameData.dt;
            gameData.player.velocity.y += deltaFriction;
        }
        if (jump)
        {
            gameData.player.velocity.y = -PLAYER_JUMP_FORCE / 2;
            if (gameData.player.onWall == 1)
            {
                gameData.player.velocity.x = -PLAYER_JUMP_FORCE / 2;
                if (Stopwatch_IsRunning(&gameData.player.wallCoyoteTime))
                {
                    gameData.player.velocity.x -= Utils_AbsFloat(gameData.player.velocityBeforeCollision.x) / 2;
                    gameData.player.velocity.y -= Utils_AbsFloat(gameData.player.velocityBeforeCollision.x) / 2;
                }
            }
            else
            {
                gameData.player.velocity.x = PLAYER_JUMP_FORCE / 2;
                if (Stopwatch_IsRunning(&gameData.player.wallCoyoteTime))
                {
                    gameData.player.velocity.x += Utils_AbsFloat(gameData.player.velocityBeforeCollision.x) / 2;
                    gameData.player.velocity.y -= Utils_AbsFloat(gameData.player.velocityBeforeCollision.x) / 2;
                }
            }
            gameData.player.onWall = false;
        }
        if (directionX != 0)
        {
            if (gameData.player.onWall != directionX)
            {
                gameData.player.onWall = false;
            }
        }
    }
    else
    {
        Stopwatch_Stop(&gameData.player.wallCoyoteTime);
        if (gameData.player.velocity.x > 0.0f)
        {
            float deltaFriction = FRICTION_AIR * gameData.dt;
            if (gameData.player.velocity.x - deltaFriction < 0.0f)
            {
                gameData.player.velocity.x = 0.0f;
            }
            else
            {
                gameData.player.velocity.x -= deltaFriction;
            }
        }
        if (gameData.player.velocity.x < 0.0f)
        {
            float deltaFriction = FRICTION_AIR * gameData.dt;
            if (gameData.player.velocity.x + deltaFriction > 0.0f)
            {
                gameData.player.velocity.x = 0.0f;
            }
            else
            {
                gameData.player.velocity.x += deltaFriction;
            }
        }
        // Apply gravity
        gameData.player.velocity.y += GRAVITY * gameData.dt;
        // Apply air movement
        if ((gameData.player.velocity.x < PLAYER_MAX_SPEED || directionX < 0)
            && (gameData.player.velocity.x > -PLAYER_MAX_SPEED || directionX > 0))
        {
            gameData.player.velocity.x += (directionX * DIRECTION_SPEED_AIR * gameData.dt);
        }
    }
    if (Stopwatch_IsRunning(&gameData.player.jumpCoyoteTime))
    {
        if (jump)
        {
            gameData.player.velocity.y = -PLAYER_JUMP_FORCE;
            Stopwatch_Stop(&gameData.player.jumpCoyoteTime);
        }
    }
    gameData.player.velocity.y =
        Utils_ClampFloat(gameData.player.velocity.y, -PLAYER_MAX_VELOCITY, PLAYER_MAX_VELOCITY);
    gameData.player.velocity.x =
        Utils_ClampFloat(gameData.player.velocity.x, -PLAYER_MAX_VELOCITY, PLAYER_MAX_VELOCITY);

    // check collisions
    gameData.player.onGround = false;

    gameData.player.entity.position.x += gameData.player.velocity.x * gameData.dt;
    for (uint32_t i = 0; i < gameData.map.platformCount; i++)
    {
        if (Collider2D_CheckCollider(&gameData.player.collider, &gameData.map.platforms[i].collider))
        {
            float playerPos    = gameData.player.entity.position.x;
            float playerSize   = gameData.player.collider.size.x;
            float platformPos  = gameData.map.platforms[i].entity.position.x;
            float platformSize = gameData.map.platforms[i].collider.size.x;
            float overlapX     = Utils_MinFloat(playerPos + playerSize, platformPos + platformSize)
                             - Utils_MaxFloat(playerPos, platformPos);

            gameData.player.velocityBeforeCollision.x = gameData.player.velocity.x;

            // Resolve X-overlap
            if (playerPos < platformPos)
            {
                gameData.player.entity.position.x -= overlapX;
                gameData.player.onWall = 1;
            }
            else
            {
                gameData.player.entity.position.x += overlapX;
                gameData.player.onWall = -1;
            }
            Stopwatch_Start(&gameData.player.wallCoyoteTime, 100);
            gameData.player.velocity.x = 0.0f;
        }
    }

    gameData.player.entity.position.y += gameData.player.velocity.y * gameData.dt;
    for (uint32_t i = 0; i < gameData.map.platformCount; i++)
    {
        if (Collider2D_CheckCollider(&gameData.player.collider, &gameData.map.platforms[i].collider))
        {
            float playerPos    = gameData.player.entity.position.y;
            float playerSize   = gameData.player.collider.size.y;
            float platformPos  = gameData.map.platforms[i].entity.position.y;
            float platformSize = gameData.map.platforms[i].collider.size.y;
            float overlapY     = Utils_MinFloat(playerPos + playerSize, platformPos + platformSize)
                             - Utils_MaxFloat(playerPos, platformPos);

            gameData.player.velocityBeforeCollision.y = gameData.player.velocity.y;

            // Resolve X-overlap
            if (playerPos < platformPos)
            {
                gameData.player.entity.position.y -= overlapY;
                gameData.player.velocity.y = 0.0f;
                gameData.player.onGround   = true;
            }
            else
            {
                gameData.player.entity.position.y += overlapY;
                gameData.player.velocity.y = 0.0f;
            }
        }
    }

    // Draw debug
    Shape2D shape;
    Shape2D_Initialize(&shape);
    shape.type                       = SHAPE2D_RECTANGLE_LINES;
    shape.parent                     = &gameData.player.entity;
    shape.position.x                 = gameData.player.collider.position.x;
    shape.position.y                 = gameData.player.collider.position.y;
    shape.rectangle.width            = gameData.player.collider.size.x;
    shape.rectangle.height           = gameData.player.collider.size.y;
    shape.color                      = (Color){ 255, 0, 0, 255 };
    shape.rectangle.outlineThickness = 1.0f;
    drawables[drawableCount].shape   = shape;
    drawables[drawableCount].type    = DRAWABLE_SHAPE;
    drawableCount++;

    for (uint32_t i = 0; i < gameData.map.platformCount; i++)
    {
        Platform* platform = &gameData.map.platforms[i];
        Shape2D_Initialize(&shape);
        shape.type                     = SHAPE2D_RECTANGLE_LINES;
        shape.parent                   = &platform->entity;
        shape.position.x               = platform->collider.position.x;
        shape.position.y               = platform->collider.position.y;
        shape.rectangle.width          = platform->collider.size.x;
        shape.rectangle.height         = platform->collider.size.y;
        shape.color                    = (Color){ 0, 255, 0, 255 };
        drawables[drawableCount].shape = shape;
        drawables[drawableCount].type  = DRAWABLE_SHAPE;
        drawableCount++;
    }
}

void MainMode_OnStart()
{
    editorTilesLoaded          = false;
    gameData.map.platformCount = 0;

    Entity2D_Initialize(&gameData.player.entity);
    Collider2D_Initialize(&gameData.player.collider);
    gameData.player.entity.position = (Vector2Float){ 0.0f, 0.0f };
    gameData.player.velocity        = (Vector2Float){ 0.0f, 0.0f };
    gameData.player.collider.parent = &gameData.player.entity;
    gameData.player.collider.size   = (Vector2Float){ 16.0f, 16.0f };

    if (g_editorTestMapData.isValid)
    {
        /* Build platforms from editor SOLID / JUMP_PLATFORM tiles */
        for (int l = 0; l < MAP_MAX_LAYERS; l++)
        {
            TileLayer* layer = &g_editorTestMapData.mapData.layers[l];
            for (int i = 0; i < (int)layer->tileCount; i++)
            {
                Tile* tile = &layer->tiles[i];
                if ((tile->type == TILE_TYPE_SOLID || tile->type == TILE_TYPE_JUMP_PLATFORM)
                    && gameData.map.platformCount < 128)
                {
                    Platform* p = &gameData.map.platforms[gameData.map.platformCount++];
                    Entity2D_Initialize(&p->entity);
                    Collider2D_Initialize(&p->collider);
                    Sprite_Initialize(&p->sprite);
                    p->entity.position.x = (float)(tile->position.x * TILE_SIZE);
                    p->entity.position.y = (float)(tile->position.y * TILE_SIZE);
                    p->collider.parent   = &p->entity;
                    p->collider.size     = (Vector2Float){ (float)TILE_SIZE, (float)TILE_SIZE };
                }
                if (tile->type == TILE_TYPE_PLAYER_SPAWN)
                {
                    gameData.player.entity.position.x = (float)(tile->position.x * TILE_SIZE);
                    gameData.player.entity.position.y = (float)(tile->position.y * TILE_SIZE);
                }
            }
        }
        /* Load tileset for visual rendering */
        editorTileAtlasBase = Texture_LoadTexture("resources/sprites/tileset.png");
        if (Texture_CreateTextureAtlas(editorTileAtlasBase, MAP_TILESET_COLS, MAP_TILESET_ROWS, editorTileTextures))
            editorTilesLoaded = true;
    }
    else
    {
        Entity2D_Initialize(&gameData.map.platforms[0].entity);
        Collider2D_Initialize(&gameData.map.platforms[0].collider);
        Sprite_Initialize(&gameData.map.platforms[0].sprite);
        gameData.map.platforms[0].entity.position = (Vector2Float){ -100.0f, 100.0f };
        gameData.map.platforms[0].collider.parent = &gameData.map.platforms[0].entity;
        gameData.map.platforms[0].collider.size   = (Vector2Float){ 200.0f, 20.0f };
        gameData.map.platformCount++;

        Entity2D_Initialize(&gameData.map.platforms[1].entity);
        Collider2D_Initialize(&gameData.map.platforms[1].collider);
        Sprite_Initialize(&gameData.map.platforms[1].sprite);
        gameData.map.platforms[1].entity.position = (Vector2Float){ 50.0f, -400.0f };
        gameData.map.platforms[1].collider.parent = &gameData.map.platforms[1].entity;
        gameData.map.platforms[1].collider.size   = (Vector2Float){ 20.0f, 500.0f };
        gameData.map.platformCount++;

        Entity2D_Initialize(&gameData.map.platforms[2].entity);
        Collider2D_Initialize(&gameData.map.platforms[2].collider);
        Sprite_Initialize(&gameData.map.platforms[2].sprite);
        gameData.map.platforms[2].entity.position = (Vector2Float){ -50.0f, -100.0f };
        gameData.map.platforms[2].collider.parent = &gameData.map.platforms[2].entity;
        gameData.map.platforms[2].collider.size   = (Vector2Float){ 20.0f, 200.0f };
        gameData.map.platformCount++;
    }

    tileAtlasBase = Texture_LoadTexture("resources/sprites/tileset.png");
    LOG_INF("MainMode: tile atlas %dx%d", tileAtlasBase.size.x, tileAtlasBase.size.y);
    if (!Texture_CreateTextureAtlas(tileAtlasBase, TILESET_ATLAS_COLS, TILESET_ATLAS_ROWS, tileTextures))
        LOG_ERR("MainMode: failed to create tile atlas");

    fontAtlasBase = Texture_LoadTexture("resources/sprites/Anikki_square_8x8.png");
    LOG_INF("MainMode: font atlas %dx%d", fontAtlasBase.size.x, fontAtlasBase.size.y);
    if (!Texture_CreateTextureAtlas(fontAtlasBase, FONT_ATLAS_COLS, FONT_ATLAS_ROWS, fontTextures))
        LOG_ERR("MainMode: failed to create font atlas");

    Entity2D_Initialize(&cameraEntity);

    Camera2D* camera = Window_GetCamera();
    camera->zoom     = 1.0f;
    camera->target   = (Vector2){ 0.0f, 0.0f };

    cameraEntity.position.x = camera->target.x;
    cameraEntity.position.y = camera->target.y;
    cameraEntity.scale      = 1.0f / camera->zoom;

    UI_Initialize(drawables, (size_t*)&drawableCount, DRAWABLE_MAX);
    UI_SetParentEntity(&cameraEntity);
}

void MainMode_OnPause()
{
}

void MainMode_Update()
{

    drawableCount = 0;
    DeltaTime_Update();
    gameData.dt = DeltaTime_GetDeltaTime();

    if (Input_IsKeyPressed(KEY_ESCAPE))
        Context_FinishMode();

    /* Camera follows player */
    UpdateGame();
    UpdateEditorTiles();
    UpdateDebug();

    Camera2D* camera        = Window_GetCamera();
    camera->target.x        = gameData.player.entity.position.x;
    camera->target.y        = gameData.player.entity.position.y;
    cameraEntity.position.x = Window_GetCamera()->target.x;
    cameraEntity.position.y = Window_GetCamera()->target.y;
    cameraEntity.scale      = 1.0f / Window_GetCamera()->zoom;
}

void MainMode_Draw()
{
    for (size_t i = 0; i < drawableCount; i++)
        Drawable_Draw(&drawables[i]);
}

void MainMode_OnStop()
{
    if (editorTilesLoaded)
    {
        Texture_UnloadTexture(&editorTileAtlasBase);
    }
    g_editorTestMapData.isValid = false;
}

void MainMode_OnResume()
{
}
