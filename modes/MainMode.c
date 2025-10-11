#include "MainMode.h"

#include "game/GameWindow.h"
#include "game/objects/Player.h"
#include "libs/engine/components/AsciiWindow.h"
#include "libs/engine/components/Audio.h"
#include "libs/engine/components/AudioPlayer.h"
#include "libs/engine/components/Texture.h"
#include "libs/engine/io/Input.h"
#include "libs/engine/misc/DeltaTime.h"
#include "libs/engine/misc/Stopwatch.h"
#include "libs/engine/misc/Utils.h"
#include "utils/SpriteDefines.h"

Mode       mainMode = { MainMode_OnStart, MainMode_OnPause, MainMode_Update, MainMode_OnStop, MainMode_OnResume };
GameWindow window;
Player     player;

void MainMode_OnStart()
{
    Texture_LoadTextureSheet("resources/sprites/Anikki_square_8x8.png", 8, 8, 16 * 16);
    Audio_LoadAudio("resources/sounds/mouse_click.wav");
    GameWindow_Initialize(&window);
    Player_Initalize(&player);
    player.gameObject.position = { 0, 0 };
    GameWindow_AddGameObject(&window, &player.gameObject);
}

void MainMode_OnPause()
{
}

void MainMode_Update()
{
    if (Input_IsKeyDown(INPUT_KEYCODE_W) || Input_IsKeyDown(INPUT_KEYCODE_S) || Input_IsKeyDown(INPUT_KEYCODE_A)
        || Input_IsKeyDown(INPUT_KEYCODE_D))
    {
        int8_t x = (Input_IsKeyDown(INPUT_KEYCODE_A) - Input_IsKeyDown(INPUT_KEYCODE_D));
        int8_t y = (Input_IsKeyDown(INPUT_KEYCODE_W) - Input_IsKeyDown(INPUT_KEYCODE_S));
        Player_Move(&player, x, y);
    }
    Player_Update(&player);
    GameWindow_Draw(&window);
}

void MainMode_OnStop()
{
    Texture_UnloadTextures();
    Audio_UnloadAudios();
}

void MainMode_OnResume()
{
}
