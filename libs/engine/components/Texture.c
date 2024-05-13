#include "Texture.h"
#include "raylib.h"
#include "engine/misc/Logger.h"
#include <string.h>
#include <stdio.h>

uint32_t sTextureCount = 0;
TextureData sTextures[MAX_TEXTURES];

bool Texture_AddTexture(Texture2D texture, const char * textureName)
{
    if(sTextureCount+1 < MAX_TEXTURES)
    {
        sTextures[sTextureCount].texture = texture;
        strcpy(sTextures[sTextureCount].textureName, textureName);
        sTextureCount++;
        return true;
    }
    LOG_ERR("Texture: Texture_AddTexture() failed, not enough space");
    return false;
}

bool Texture_LoadTexture(const char *fileName)
{
    if(fileName == NULL)
    {
        return false;
    }
    Texture2D texture = LoadTexture(fileName);
    if(texture.id > 0)//Texture loaded correctly(?)
    {
        return Texture_AddTexture(texture, GetFileNameWithoutExt(fileName));
    }
    return false;
}

uint8_t Texture_LoadTextureSheet(const char *fileName, uint32_t textureWidth, uint32_t textureHeight)
{
    if(fileName == NULL)
    {
        return 0;
    }
    uint8_t completed = 0;
    Image baseImage = LoadImage(fileName);
    for(uint32_t y = 0; y < baseImage.height/textureHeight; y++)
    {
        for(uint32_t x = 0; x < baseImage.width/textureWidth; x++)
        {
            Rectangle rect;
            rect.x = x * textureWidth;
            rect.y = y * textureHeight;
            rect.width = textureWidth;
            rect.height = textureHeight;
            Image image = ImageFromImage(baseImage, rect);
            Texture texture = LoadTextureFromImage(image);
            if(texture.id > 0)//Texture loaded correctly(?)
            {
                char name[MAX_NAME];
                sprintf(name, "%s_%d", GetFileNameWithoutExt(fileName), (y * baseImage.height/textureHeight) + x);
                if(Texture_AddTexture(texture, name))
                {
                    completed++;
                }
            }
        } 
    }
    return completed;
}

bool Texture_UnloadTextureByName(const char *textureName)
{
    if(textureName == NULL)
    {
        return false;
    }
    bool moveTextures = false;
    for(uint32_t i = 0; i < sTextureCount; i++)
    {
        if((strcmp(textureName, sTextures[i].textureName) == 0) && moveTextures == false)
        {
            moveTextures = true;
            UnloadTexture(sTextures[i].texture);
        }
        if(moveTextures && i != sTextureCount-1)
        {
            sTextures[i] = sTextures[i+1];
        }
    }
    if(moveTextures)
    {
        sTextureCount--;
    }
    return moveTextures;
}

bool Texture_UnloadTextureById(uint32_t textureId)
{
    bool moveTextures = false;
    for(uint32_t i = 0; i < sTextureCount; i++)
    {
        if(sTextures[i].texture.id == textureId && moveTextures == false)
        {
            moveTextures = true;
            UnloadTexture(sTextures[i].texture);
        }
        if(moveTextures && i != sTextureCount-1)
        {
            sTextures[i] = sTextures[i+1];
        }
    }
    if(moveTextures)
    {
        sTextureCount--;
    }
    return moveTextures;
}

void Texture_UnloadTextures()
{
    for(uint32_t i = 0; i < sTextureCount; i++)
    {
        UnloadTexture(sTextures[i].texture);
    }
    sTextureCount = 0;
}

uint32_t Texture_GetCount()
{
    return sTextureCount;
}

TextureData * Texture_GetTextures()
{
    return sTextures;
}

Texture2D * Texture_GetTextureByName(const char *textureName)
{
    for(uint32_t i = 0; i < sTextureCount; i++)
    {
        if(strcmp(sTextures[i].textureName, textureName) == 0)
        {
            return &sTextures[i].texture;
        }
    }
    LOG_ERR("Texture: Texture_GetTextureByName() failed, texture of name `%s` not found", textureName);
    return NULL;
}