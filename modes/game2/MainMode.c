#include "MainMode.h"

#include "ashes/ash_components.h"
#include "ashes/ash_context.h"
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

bool IsPlayerOnGround()
{
    return gameData.playerPos.y >= 0.0f;
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

    TextureData texture = Texture_LoadTexture("texturepack.png");
    if (!Texture_CreateTextureAtlas(texture, 16, 16, textures))
    {
        LOG_ERR("Failed to create texture atlas");
    }
}

void MainMode_OnPause()
{
}

void MainMode_Update()
{
    DeltaTime_Update();
    gameData.dt = DeltaTime_GetDeltaTime();

    int  direction = { -Input_IsKeyPressed(KEY_A) + Input_IsKeyPressed(KEY_D) };
    bool isJumping = Input_IsKeyPressed(KEY_SPACE);

    if (IsPlayerOnGround())
    {
        gameData.playerVel.y = 0.0f;
        gameData.playerVel.x += direction * gameData.dt * DIRECTION_SPEED_GROUND;
        if (isJumping)
        {
            gameData.playerVel.y = 5.0f;  // Jump impulse
        }
        else
        {
            // Apply friction
            if (gameData.playerVel.x > 0.0f)
            {
                gameData.playerVel.x -= FRICTION_GROUND * gameData.dt;
            }
            else if (gameData.playerVel.x < 0.0f)
            {
                gameData.playerVel.x += FRICTION_GROUND * gameData.dt;
            }
        }
    }
    else
    {
        gameData.playerVel.y -= 9.8f * gameData.dt;
        gameData.playerVel.x += direction * gameData.dt * DIRECTION_SPEED_AIR;
    }
    // Draw part
    Sprite sprite;
    Sprite_Initialize(&sprite);
}

void MainMode_OnStop()
{
    Texture_UnloadTexture(&textures[0]);
}

void MainMode_OnResume()
{
}
