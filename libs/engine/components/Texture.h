#ifndef LIBS_ENGINE_TEXTURE_H
#define LIBS_ENGINE_TEXTURE_H
#include "raylib.h"

#include <stdint.h>
#include <stdlib.h>
#define TEXTURE_MAX_COUNT          1024
#define TEXTURE_MAX_NAME           32
#define TEXTURE_ALPHA_COLOR        { 0xff, 0xff, 0x00, 0xff }
#define TEXTURE_INFO_FILE_MAX_NAME 64
#define TEXTURE_INFO_LINE_MAX      128

typedef struct TextureData
{
    Texture2D texture;
    char      textureName[TEXTURE_MAX_NAME];
} TextureData;

bool    Texture_LoadTexture(const char* fileName);
uint8_t Texture_LoadTextureSheet(const char* fileName, uint32_t textureWidth, uint32_t textureHeight,
                                 uint32_t texturesCount);
uint8_t Texture_LoadTextureSheetWithInfo(const char* fileName);

bool    Texture_UnloadTextureByName(const char* textureName);
bool    Texture_UnloadTextureById(uint32_t textureId);
void    Texture_UnloadTextures();

uint32_t     Texture_GetCount();
TextureData* Texture_GetTextures();
Texture2D*   Texture_GetTextureByName(const char* textureName);
Texture2D*   Texture_GetTextureById(uint32_t textureId);
#endif
