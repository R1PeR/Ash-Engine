#include "MainMode.h"
#include "libs/engine/components/Texture.h"
#include "libs/engine/misc/Stopwatch.h"
#include "libs/engine/io/Input.h"
#include "libs/engine/misc/DeltaTime.h"
#include "libs/engine/components/AnimatedSprite.h"
// #include "libs/engine/components/Collider2D.h"
#include "libs/engine/components/Audio.h"
#include "libs/engine/components/AudioPlayer.h"

AnimationData player_move;

typedef struct GameObject
{
    Entity2D entity;
    AnimatedSprite animatedSprite;
    // Collider2D collider;
} Object;

Object object;
Stopwatch timer;
Mode mainMode = {MainMode_OnStart, MainMode_OnPause, MainMode_Update, MainMode_OnStop, MainMode_OnResume};
void MainMode_OnStart()
{
    Texture_LoadTextureSheet("resources/sprites/player.png", 32, 32);
    Audio_LoadAudio("resources/sounds/mouse_click.wav");
    
    Entity2D_Initialize(&object.entity);
    AnimatedSprite_Initialize(&object.animatedSprite);
    // Collider2D_Initialize(&object.collider);
    object.animatedSprite.sprite.parent = &object.entity;
    object.animatedSprite.sprite.currentTexture = Texture_GetTextureByName("player_0");
    // object.collider.parent = &object.entity;
    // object.collider.size.x = 1.0f;
    // object.collider.size.y = 1.0f;
    AnimatedSprite_SetAnimationDataFromTextureSheet(&player_move, "player", 0, 10);

    AnimatedSprite_Add(&object.animatedSprite);
    Entity2D_Add(&object.entity);

    // Collider2D_Add(&object.collider);
}

void MainMode_OnPause()
{

}

void MainMode_Update()
{
    if(Input_IsMouseButtonPressed(INPUT_MOUSE_BUTTON_LEFT))
    {
        AudioPlayer_PlaySoundByName("mouse_click");
    }
    if(Input_IsKeyDown(INPUT_KEYCODE_W))
    {
        object.entity.position.y -= 0.1f * DeltaTime_GetDeltaTime();
    }
    if(Input_IsKeyDown(INPUT_KEYCODE_S))
    {
        object.entity.position.y += 0.1f * DeltaTime_GetDeltaTime();
    }
    if(Input_IsKeyDown(INPUT_KEYCODE_A))
    {
        object.entity.position.x -= 0.1f * DeltaTime_GetDeltaTime();
    }  
    if(Input_IsKeyDown(INPUT_KEYCODE_D))
    {
        object.entity.position.x += 0.1f * DeltaTime_GetDeltaTime();
    }
}

void MainMode_OnStop()
{
    Texture_UnloadTextures();
    Audio_UnloadAudios();
}

void MainMode_OnResume()
{

}
