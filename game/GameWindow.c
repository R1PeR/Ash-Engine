#include "GameWindow.h"

#include "engine/components/AsciiWindow.h"
#include "engine/misc/Logger.h"
#include "game/GameObject.h"
#include "utils/SpriteDefines.h"


void GameWindow_Initialize(GameWindow* window)
{
    window->border.top[0]    = FONT_BORDER_D_TOP_LEFT;
    window->border.top[1]    = FONT_BORDER_D_TOP;
    window->border.top[2]    = FONT_BORDER_D_TOP_RIGHT;
    window->border.middle[0] = FONT_BORDER_D_LEFT;
    window->border.middle[1] = ' ';
    window->border.middle[2] = FONT_BORDER_D_RIGHT;
    window->border.bottom[0] = FONT_BORDER_D_BOTTOM_LEFT;
    window->border.bottom[1] = FONT_BORDER_D_BOTTOM;
    window->border.bottom[2] = FONT_BORDER_D_BOTTOM_RIGHT;

    Entity2D_Initialize(&window->window.entity);

    window->window.height       = GAMEWINDOW_SIZE;
    window->window.width        = GAMEWINDOW_SIZE;
    window->window.windowBuffer = window->windowBuffer;
    window->window.spriteBuffer = window->spriteBuffer;
    window->window.entity.scale = 2.0f;
    AsciiWindow_Initalize(&window->window, "Anikki_square_8x8");
    window->window.entity.position.x = static_cast<float>(window->window.width * window->window.spriteWidth)
                                       * window->window.entity.scale / 2.0 * -1.0;
    window->window.entity.position.y = static_cast<float>(window->window.height * window->window.spriteHeight)
                                       * window->window.entity.scale / 2.0 * -1.0;
    window->worldPosition = { 0, 0 };
    window->selected      = 0;
}

void GameWindow_AddGameObject(GameWindow* window, GameObject* object)
{
    if (window->objectCount < GAMEWINDOW_MAX_GAMEOBJECTS)
    {
        window->objects[window->objectCount] = object;
        window->objectCount++;
    }
}

void GameWindow_DeleteGameObject(GameWindow* window, GameObject* object)
{
    for (uint32_t i = 0; i < window->objectCount; i++)
    {
        if (object == window->objects[i])
        {
            for (uint32_t j = 0; j < window->objectCount - i - 1; j++)
            {
                window->objects[i + j] = window->objects[i + j + 1];
            }
            break;
        }
    }
}

void GameWindow_Selection(GameWindow* gameWindow, uint8_t selectionCount, Vector2Int* selection)
{
    if (selectionCount > GAMEWINDOW_MAX_SELECTION)
    {
        LOG_INF("GameWindow: Selection exceeded max limit");
        return;
    }
    for (uint8_t i = 0; i < selectionCount; i++)
    {
        gameWindow->selection[i] = selection[i];
    }
}

void GameWindow_Draw(GameWindow* gameWindow)
{
    AsciiWindow_DrawFill(&gameWindow->window, FONT_FILL_A);
    AsciiWindow_DrawBorder(&gameWindow->window, gameWindow->border);
    for (uint32_t i = 0; i < gameWindow->objectCount; i++)
    {
        if (gameWindow->objects[i] != nullptr)
        {
            if (gameWindow->objects[i]->textureId != 0)
            {
                GameObject* object    = gameWindow->objects[i];
                uint8_t     worldSize = ((GAMEWINDOW_SIZE) / 2);
                if (abs(object->position.x - gameWindow->worldPosition.x) < worldSize
                    && abs(object->position.y - gameWindow->worldPosition.y) < worldSize)
                {
                    // LOG_INF("%d %d", -object->position.x + worldSize, -object->position.y + worldSize);
                    AsciiWindow_SetCell(&gameWindow->window, -object->position.x + worldSize,
                                        -object->position.y + worldSize, object->textureId);
                }
            }
        }
    }
    AsciiWindow_Draw(&gameWindow->window);
    if (gameWindow->selected != 0)
    {
        for (uint32_t i = 0; i < gameWindow->selected; i++)
        {
            AsciiWindow_SetCell(&gameWindow->foreground, gameWindow->selection[i].x, gameWindow->selection[i].y,
                                gameWindow->selectionChar);
        }
        // read selection
    }
    AsciiWindow_Draw(&gameWindow->foreground);
}
