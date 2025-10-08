#ifndef GAME_GAMEWINDOW_H
#define GAME_GAMEWINDOW_H
#include "GameObject.h"
#include "engine/components/AsciiWindow.h"

#include <stdint.h>
#define GAMEWINDOW_MAX_GAMEOBJECTS 256
#define GAMEWINDOW_SIZE            31
#define GAMEWINDOW_MAX_SELECTION   8

typedef struct GameWindow
{
    Vector2Int        worldPosition;
    uint8_t           windowBuffer[GAMEWINDOW_SIZE * GAMEWINDOW_SIZE];
    Sprite            spriteBuffer[GAMEWINDOW_SIZE * GAMEWINDOW_SIZE];
    AsciiWindow       window;
    AsciiWindowBorder border;
    GameObject*       objects[GAMEWINDOW_MAX_GAMEOBJECTS];
    uint32_t          objectCount;
    uint8_t           selected;
    Vector2Int        selection[GAMEWINDOW_MAX_SELECTION];
    uint32_t          selectionChar;
    AsciiWindow       foreground;
} GameWindow;

void GameWindow_Initialize(GameWindow* window);
void GameWindow_AddGameObject(GameWindow* window, GameObject* object);
void GameWindow_DeleteGameObject(GameWindow* window, GameObject* object);
void GameWindow_Draw(GameWindow* gameWindow);

#endif
