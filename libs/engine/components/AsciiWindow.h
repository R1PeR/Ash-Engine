#ifndef LIBS_ENGINE_ASCIIWINDOW_H
#define LIBS_ENGINE_ASCIIWINDOW_H
#include "Sprite.h"

#include <stdint.h>
#include <stdlib.h>
// #define ASCIIWINDOW_MAX_SIZE 64
#define ASCIIWINDOW_MAX_TEXURES 256
#define ASCIIWINDOW_ASCII_START 0

typedef struct AsciiWindow
{
    Entity2D entity;
    uint8_t* windowBuffer;
    Sprite*  spriteBuffer;
    Texture* textureBuffer[ASCIIWINDOW_MAX_TEXURES];
    Vector2  position;
    uint32_t width;
    uint32_t height;
    uint32_t spriteWidth;
    uint32_t spriteHeight;
} AsciiWindow;

typedef struct AsciiSubWindow
{
    AsciiWindow* parent;
    Vector2      position;
    uint32_t     width;
    uint32_t     height;
} AsciiSubWindow;

typedef struct AsciiWindowBorder
{
    uint32_t top[3];
    uint32_t middle[3];
    uint32_t bottom[3];
} AsciiWindowBorder;

void     AsciiWindow_Initalize(AsciiWindow* window, const char* textureName);
void     AsciiWindow_SetCharacter(AsciiWindow* window, uint32_t x, uint32_t y, char c);
char     AsciiWindow_GetCharacter(AsciiWindow* window, uint32_t x, uint32_t y);
void     AsciiWindow_SetCell(AsciiWindow* window, uint32_t x, uint32_t y, uint32_t c);
uint32_t AsciiWindow_GetCell(AsciiWindow* window, uint32_t x, uint32_t y);
void     AsciiWindow_Clear(AsciiWindow* window);
void     AsciiWindow_Draw(AsciiWindow* window);

void AsciiWindow_DrawBorder(AsciiWindow* window, AsciiWindowBorder border);
void AsciiWindow_DrawFill(AsciiWindow* window, uint8_t fill);
void AsciiWindow_DrawString(AsciiWindow* window, uint8_t x, uint8_t y, const char* string);

void     AsciiSubWindow_Initalize(AsciiSubWindow* window);
void     AsciiSubWindow_SetCell(AsciiSubWindow* window, uint32_t x, uint32_t y, uint32_t c);
uint32_t AsciiSubWindow_GetCell(AsciiSubWindow* window, uint32_t x, uint32_t y);
void     AsciiSubWindow_SetCharacter(AsciiSubWindow* window, uint32_t x, uint32_t y, char c);
char     AsciiSubWindow_GetCharacter(AsciiSubWindow* window, uint32_t x, uint32_t y);
void     AsciiSubWindow_Clear(AsciiSubWindow* window);

void AsciiSubWindow_DrawBorder(AsciiSubWindow* window, AsciiWindowBorder border);
void AsciiSubWindow_DrawFill(AsciiSubWindow* window, uint32_t fill);
void AsciiSubWindow_DrawString(AsciiSubWindow* window, const char* string);

#endif
