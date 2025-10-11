#include "AsciiWindow.h"

#include "Texture.h"
#include "engine/misc/Logger.h"

#include <cstdint>
#include <stdio.h>


void AsciiWindow_Initalize(AsciiWindow* window, const char* textureName)
{
    char       name[TEXTURE_MAX_NAME];
    uint32_t   textureId = 0;
    Texture2D* texture;
    while (true)
    {
        sprintf(name, "%s_%d", textureName, textureId);
        texture = Texture_GetTextureByName(name);
        if (texture == NULL)
        {
            break;
        }
        window->textureBuffer[textureId] = texture;
        textureId++;
    }
    AsciiWindow_Clear(window);
    for (uint32_t i = 0; i < window->height * window->width; i++)
    {
        Sprite_Initialize(&window->spriteBuffer[i]);
    }
    Entity2D_Add(&window->entity);
    window->spriteHeight = window->textureBuffer[0]->height;
    window->spriteWidth  = window->textureBuffer[0]->width;
    for (uint32_t i = 0; i < window->height; i++)
    {
        for (uint32_t j = 0; j < window->width; j++)
        {
            window->spriteBuffer[i * window->width + j].position.x = window->textureBuffer[0]->width
                                                                     * window->spriteBuffer[i * window->width + j].scale
                                                                     * window->entity.scale * j;
            window->spriteBuffer[i * window->width + j].position.y = window->textureBuffer[0]->height
                                                                     * window->spriteBuffer[i * window->width + j].scale
                                                                     * window->entity.scale * i;
            window->spriteBuffer[i * window->width + j].currentTexture = window->textureBuffer[0];
            window->spriteBuffer[i * window->width + j].parent         = &window->entity;
            Sprite_Add(&window->spriteBuffer[i * window->width + j]);
        }
    }
}

void AsciiWindow_SetCharacter(AsciiWindow* window, uint32_t x, uint32_t y, char c)
{
    AsciiWindow_SetCell(window, x, y, static_cast<uint32_t>(c) - ASCIIWINDOW_ASCII_START);
}

char AsciiWindow_GetCharacter(AsciiWindow* window, uint32_t x, uint32_t y)
{
    return static_cast<char>(AsciiWindow_GetCell(window, x, y) + ASCIIWINDOW_ASCII_START);
}

void AsciiWindow_SetCell(AsciiWindow* window, uint32_t x, uint32_t y, uint32_t value)
{
    window->windowBuffer[y * window->width + x] = value;
}

uint32_t AsciiWindow_GetCell(AsciiWindow* window, uint32_t x, uint32_t y)
{
    return window->windowBuffer[y * window->width + x];
}

void AsciiWindow_Clear(AsciiWindow* window)
{
    for (uint32_t i = 0; i < window->height * window->width; i++)
    {
        window->windowBuffer[i] = 0;
    }
}

void AsciiWindow_Draw(AsciiWindow* window)
{
    for (uint32_t i = 0; i < window->height; i++)
    {
        for (uint32_t j = 0; j < window->width; j++)
        {
            uint32_t currentChar                                       = AsciiWindow_GetCell(window, j, i);
            window->spriteBuffer[i * window->width + j].currentTexture = window->textureBuffer[currentChar];
        }
    }
}

void AsciiWindow_DrawBorder(AsciiWindow* window, AsciiWindowBorder border)
{
    for (uint32_t i = 0; i < window->height; i++)
    {
        for (uint32_t j = 0; j < window->width; j++)
        {
            if (i == 0 && j == 0)
            {
                AsciiWindow_SetCell(window, j, i, border.top[0]);
            }
            else if (i == 0 && j == window->width - 1)
            {
                AsciiWindow_SetCell(window, j, i, border.top[2]);
            }
            else if (i == window->height - 1 && j == 0)
            {
                AsciiWindow_SetCell(window, j, i, border.bottom[0]);
            }
            else if (i == window->height - 1 && j == window->width - 1)
            {
                AsciiWindow_SetCell(window, j, i, border.bottom[2]);
            }
            else if (i == 0 && j != 0 && j != window->width - 1)
            {
                AsciiWindow_SetCell(window, j, i, border.top[1]);
            }
            else if (i == window->height - 1 && j != 0 && j != window->width - 1)
            {
                AsciiWindow_SetCell(window, j, i, border.bottom[1]);
            }
            else if (j == 0 && i != 0 && i != window->height - 1)
            {
                AsciiWindow_SetCell(window, j, i, border.middle[0]);
            }
            else if (j == window->width - 1 && i != 0 && i != window->height - 1)
            {
                AsciiWindow_SetCell(window, j, i, border.middle[2]);
            }
        }
    }
}

void AsciiWindow_DrawFill(AsciiWindow* window, uint8_t fill)
{
    for (uint32_t i = 0; i < window->height; i++)
    {
        for (uint32_t j = 0; j < window->width; j++)
        {
            AsciiWindow_SetCharacter(window, j, i, fill);
        }
    }
}

void AsciiWindow_DrawString(AsciiWindow* window, uint8_t x, uint8_t y, const char* string)
{
    uint32_t position = 0;
    while (string[position] != '\0')
    {
        AsciiWindow_SetCharacter(window, x, y, string[position]);
        x++;
        position++;
        if (x >= window->width)
        {
            x = 0;
            y++;
        }
        if (y >= window->height)
        {
            break;
        }
    }
}
