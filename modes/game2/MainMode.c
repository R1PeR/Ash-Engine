#include "MainMode.h"

#include "ashes/ash_components.h"
#include "ashes/ash_context.h"
#include "ashes/ash_debug.h"
#include "ashes/ash_io.h"
#include "ashes/ash_misc.h"

#define DIRECTION_SPEED_GROUND 5.0f
#define DIRECTION_SPEED_AIR    2.0F
#define FRICTION_GROUND        10.0f
#define FRICTION_AIR           1.0f
#define GRAVITY                9.8f

Mode mainMode = MODE_FROM_CLASSNAME(MainMode);

enum PlayerState
{
    PlayerState_Idle,
    PlayerState_Moving,
    PlayerState_InAir,
    PlayerState_Dead
};

struct GameData
{
    float        dt;
    Vector2Float playerPos;
    Vector2Float playerVel;
    PlayerState  playerState;
} gameData;

TextureData textures[512];
TextureData texture;
Sprite      sprites[100000];

bool IsPlayerOnGround()
{
    return gameData.playerPos.y <= 0.0f;
}

bool IsPlayerOnWall()
{
    return gameData.playerPos.x <= -10.0f || gameData.playerPos.x >= 10.0f;
}

void UpdatePlayer()
{
}

void MainMode_OnStart()
{
    gameData.playerPos   = (Vector2Float){ 0.0f, 0.0f };
    gameData.playerVel   = (Vector2Float){ 0.0f, 0.0f };
    gameData.playerState = PlayerState_Idle;

    texture = Texture_LoadTexture("resources/sprites/font.png");
    LOG_INF("Loaded texture: %s, width: %d, height: %d", "resources/sprites/player.png", texture.size.x,
            texture.size.y);
    if (!Texture_CreateTextureAtlas(texture, 16, 16, textures))
    {
        LOG_ERR("Failed to create texture atlas");
    }
    for (uint32_t i = 0; i < 100000; i++)
    {
        Sprite_Initialize(&sprites[i]);
        sprites[i].currentTexture = &textures[Utils_GetRandomInRangeInteger(0, 64)];
        sprites[i].scale          = 1.0f;
        sprites[i].position       = { Utils_GetRandomInRangeFloat(-500, 500), Utils_GetRandomInRangeFloat(-300, 300) };
    }
}

void MainMode_OnPause()
{
}

void MainMode_Update()
{
    // DeltaTime_Update();
    // gameData.dt = DeltaTime_GetDeltaTime();
    //
    // int  direction = { -Input_IsKeyDown(KEY_A) + Input_IsKeyDown(KEY_D) };
    // bool isJumping = Input_IsKeyDown(KEY_SPACE);
    //
    // gameData.playerPos.x += direction * gameData.dt * 50.0f;

    // if (IsPlayerOnGround())
    // {
    //     gameData.playerVel.y = 0.0f;
    //     gameData.playerVel.x += direction * gameData.dt * DIRECTION_SPEED_GROUND;
    //     if (isJumping)
    //     {
    //         gameData.playerVel.y = 5.0f;  // Jump impulse
    //     }
    //     else
    //     {
    //         // Apply friction
    //         if (gameData.playerVel.x > 0.0f)
    //         {
    //             gameData.playerVel.x -= FRICTION_GROUND * gameData.dt;
    //         }
    //         else if (gameData.playerVel.x < 0.0f)
    //         {
    //             gameData.playerVel.x += FRICTION_GROUND * gameData.dt;
    //         }
    //     }
    // }
    // else
    // {
    //     gameData.playerVel.y -= 9.8f * gameData.dt;
    //     gameData.playerVel.x += direction * gameData.dt * DIRECTION_SPEED_AIR;
    // }
    // Draw part
    for (uint32_t i = 0; i < 100000; i++)
    {
        sprites[i].position = { Utils_GetRandomInRangeFloat(-500, 500), Utils_GetRandomInRangeFloat(-300, 300) };
        Sprite_Draw(&sprites[i]);
    }
}

void MainMode_OnStop()
{
    Texture_UnloadTexture(&textures[0]);
}

void MainMode_OnResume()
{
}
