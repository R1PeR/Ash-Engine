#ifndef LIBS_ENGINE_TEXTURE_H
#define LIBS_ENGINE_TEXTURE_H
#include "raylib.h"
#include "utils.h"

#include <stdint.h>
#include <stdlib.h>
#define TEXTURE_MAX_COUNT   1024
#define TEXTURE_MAX_NAME    32
#define TEXTURE_ALPHA_COLOR { 0xff, 0xff, 0x00, 0xff }

typedef struct TextureData
{
    Texture2D texture;
    char      textureName[TEXTURE_MAX_NAME];
} TextureData;

bool    Texture_LoadTexture(const char* fileName);
uint8_t Texture_LoadTextureSheet(const char* fileName, uint32_t textureWidth, uint32_t textureHeight,
                                 uint32_t texturesCount);
bool    Texture_UnloadTextureByName(const char* textureName);
bool    Texture_UnloadTextureById(uint32_t textureId);
void    Texture_UnloadTextures();

uint32_t     Texture_GetCount();
TextureData* Texture_GetTextures();
Texture2D*   Texture_GetTextureByName(const char* textureName);
#endif
