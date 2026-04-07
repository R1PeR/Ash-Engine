#include "MainMode.h"

#include "ashes/ash_components.h"
#include "ashes/ash_context.h"
#include "ashes/ash_debug.h"
#include "ashes/ash_io.h"
#include "ashes/ash_misc.h"

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

TextureData textures[512];
TextureData texture;
Sprite      sprites[128];
uint8_t     spriteCount = 0;

void DrawDebug()
{
    Vector2Float originPoint = { -400, -300 };
    DrawText(
        TextFormat("Player Pos: (%.2f, %.2f)", gameData.player.entity.position.x, gameData.player.entity.position.y),
        originPoint.x, originPoint.y, 20, WHITE);
    DrawText(TextFormat("Player Vel: (%.2f, %.2f)", gameData.player.velocity.x, gameData.player.velocity.y),
             originPoint.x, originPoint.y + 30, 20, WHITE);
    DrawText(TextFormat("Player onGround: %s", gameData.player.onGround ? "true" : "false"), originPoint.x,
             originPoint.y + 60, 20, WHITE);
    DrawText(TextFormat("Player onWall: %s", (gameData.player.onWall != 0) ? "true" : "false"), originPoint.x,
             originPoint.y + 90, 20, WHITE);
    //print coyote times
    DrawText(TextFormat("Player jumpCoyoteTime: %s", Stopwatch_IsRunning(&gameData.player.jumpCoyoteTime) ? "true" : "false"), originPoint.x,
             originPoint.y + 120, 20, WHITE);
    DrawText(TextFormat("Player wallCoyoteTime: %s", Stopwatch_IsRunning(&gameData.player.wallCoyoteTime) ? "true" : "false"), originPoint.x,
             originPoint.y + 150, 20, WHITE);
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
                    gameData.player.velocity.x -= Utils_AbsFloat(gameData.player.velocityBeforeCollision.x)/2;
                    gameData.player.velocity.y -= Utils_AbsFloat(gameData.player.velocityBeforeCollision.x)/2;
                }
            }
            else
            {
                gameData.player.velocity.x = PLAYER_JUMP_FORCE / 2;
                if (Stopwatch_IsRunning(&gameData.player.wallCoyoteTime))
                {
                    gameData.player.velocity.x += Utils_AbsFloat(gameData.player.velocityBeforeCollision.x)/2;
                    gameData.player.velocity.y -= Utils_AbsFloat(gameData.player.velocityBeforeCollision.x)/2;
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
        if (gameData.player.velocity.x < PLAYER_MAX_SPEED && gameData.player.velocity.x > -PLAYER_MAX_SPEED)
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


    gameData.player.entity.position.y += gameData.player.velocity.y * gameData.dt;
    gameData.player.entity.position.x += gameData.player.velocity.x * gameData.dt;

    // check collisions
    gameData.player.onGround = false;
    for (uint32_t i = 0; i < gameData.map.platformCount; i++)
    {
        if (Collider2D_CheckCollider(&gameData.player.collider, &gameData.map.platforms[i].collider))
        {
            Vector2Float playerPos    = { gameData.player.entity.position.x, gameData.player.entity.position.y };
            Vector2Float playerSize   = { gameData.player.collider.size.x, gameData.player.collider.size.y };
            Vector2Float platformPos  = { gameData.map.platforms[i].entity.position.x,
                                          gameData.map.platforms[i].entity.position.y };
            Vector2Float platformSize = { gameData.map.platforms[i].collider.size.x,
                                          gameData.map.platforms[i].collider.size.y };

            float overlapX = Utils_MinFloat(playerPos.x + playerSize.x, platformPos.x + platformSize.x)
                             - Utils_MaxFloat(playerPos.x, platformPos.x);
            float overlapY = Utils_MinFloat(playerPos.y + playerSize.y, platformPos.y + platformSize.y)
                             - Utils_MaxFloat(playerPos.y, platformPos.y);

            gameData.player.velocityBeforeCollision.x = gameData.player.velocity.x;
            gameData.player.velocityBeforeCollision.y = gameData.player.velocity.y;
            if (overlapX < overlapY)
            {
                if (playerPos.x < platformPos.x)
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
            else
            {
                if (playerPos.y < platformPos.y)
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
    };

    // Update sprites
    sprites[spriteCount].position.x     = gameData.player.entity.position.x;
    sprites[spriteCount].position.y     = gameData.player.entity.position.y;
    sprites[spriteCount].currentTexture = &textures[0];
    sprites[spriteCount].scale          = 2.0f;
    spriteCount++;

    // Draw debug
    Collider2D_DrawDebug(&gameData.player.collider);
    for (uint32_t i = 0; i < gameData.map.platformCount; i++)
    {
        Collider2D_DrawDebug(&gameData.map.platforms[i].collider);
    }
}

void MainMode_OnStart()
{
    gameData.map.platformCount = 0;
    Entity2D_Initialize(&gameData.player.entity);
    Collider2D_Initialize(&gameData.player.collider);
    gameData.player.entity.position = (Vector2Float){ 0.0f, 0.0f };
    gameData.player.velocity        = (Vector2Float){ 0.0f, 0.0f };
    gameData.player.collider.parent = &gameData.player.entity;
    gameData.player.collider.size   = (Vector2Float){ 16.0f, 16.0f };

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

    texture = Texture_LoadTexture("resources/sprites/font.png");
    LOG_INF("Loaded texture: %s, width: %d, height: %d", "resources/sprites/player.png", texture.size.x,
            texture.size.y);
    if (!Texture_CreateTextureAtlas(texture, 16, 16, textures))
    {
        LOG_ERR("Failed to create texture atlas");
    }
    for (uint32_t i = 0; i < 128; i++)
    {
        Sprite_Initialize(&sprites[i]);
    }
}

void MainMode_OnPause()
{
}

void MainMode_Update()
{
    spriteCount = 0;
    DeltaTime_Update();
    gameData.dt = DeltaTime_GetDeltaTime();

    UpdateGame();

    // Draw part
    // for (uint32_t i = 0; i < spriteCount; i++)
    // {
    //     Sprite_Draw(&sprites[i]);
    // }
    DrawDebug();
}

void MainMode_OnStop()
{
    Texture_UnloadTexture(&textures[0]);
}

void MainMode_OnResume()
{
}
