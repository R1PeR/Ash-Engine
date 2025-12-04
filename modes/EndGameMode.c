#include "EndGameMode.h"

#include "EndGameMode.h"
#include "engine/components/Audio.h"
#include "engine/components/Sprite.h"
#include "engine/components/Texture.h"
#include "engine/io/Window.h"
#include "engine/misc/Logger.h"
#include "engine/misc/Utils.h"
#include "raylib.h"
#include "utils/UI.h"

#include <string.h>
#include <stdio.h>

Mode endGameMode = MODE_FROM_CLASSNAME(EndGameMode);
Entity2D cameraEntity;
Sprite sprites[32];

void EndGameMode_OnStart()
{
    UI_Init(&cameraEntity);
    Sprite_SetPool(sprites, 32);
}

void EndGameMode_OnPause()
{
}

void EndGameMode_Update()
{
    Sprite_Clear();
    cameraEntity.position = Window_GetCamera()->target;
    cameraEntity.scale    = 1.0f / Window_GetCamera()->zoom;

    char buffer[32];

    snprintf(buffer, sizeof(buffer), "FPS: %d", GetFPS());
    Text text;
    text.position   = (Vector2Float){ -540.0f, -340.0f };
    text.buffer     = buffer;
    text.bounds     = (Rectangle){ 0.0f, 0.0f, 100.0f, 30.0f };
    text.bufferSize = strlen(buffer);
    text.scale      = 2.0f;
    if (UI_Text(&text, "Anikki_square_8x8"))
    {
        LOG_INF("Text clicked!");
    }

    snprintf(buffer, sizeof(buffer), "GameOver");
    text.position   = (Vector2Float){ -0.0f, -0.0f };
    text.buffer     = buffer;
    text.bounds     = (Rectangle){ 0.0f, 0.0f, 100.0f, 30.0f };
    text.bufferSize = strlen(buffer);
    text.scale      = 4.0f;
    if (UI_Text(&text, "Anikki_square_8x8"))
    {
        LOG_INF("Text clicked!");
    }
}

void EndGameMode_OnStop()
{

}

void EndGameMode_OnResume()
{
}
