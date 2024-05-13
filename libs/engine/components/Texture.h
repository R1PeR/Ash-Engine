#ifndef LIBS_ENGINE_TEXTURE_H
#define LIBS_ENGINE_TEXTURE_H
#include <stdint.h>
#include <stdlib.h>
#include "raylib.h"
#include "utils.h"
#define MAX_TEXTURES 1024
#define MAX_NAME 32

typedef struct TextureData 
{
    Texture2D texture;
    char textureName[MAX_NAME];
} TextureData;

bool Texture_LoadTexture(const char *fileName);
uint8_t Texture_LoadTextureSheet(const char *fileName, uint32_t textureWidth, uint32_t textureHeight);
bool Texture_UnloadTextureByName(const char *textureName);
bool Texture_UnloadTextureById(uint32_t textureId);
void Texture_UnloadTextures();

uint32_t Texture_GetCount();
TextureData * Texture_GetTextures();
Texture2D * Texture_GetTextureByName(const char *textureName);
#endif