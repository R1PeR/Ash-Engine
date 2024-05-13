#include "Sprite.h"
#include "engine/misc/Logger.h"

Updatable spriteUpdatable = {Sprite_Update};
uint32_t sSpriteCount = 0;
Sprite * sSprites[MAX_SPRITES];

void Sprite_Initialize(Sprite * spr)
{
    spr->currentTexture = NULL;
    spr->id = 0;
    spr->position.x = 0;
    spr->position.y = 0;
    spr->isVisible = true;
    spr->rotation = 0;
    spr->scale = 1.0f;
    spr->zOrder = 0;
}

bool Sprite_Add(Sprite * spr)
{
    if(sSpriteCount < MAX_SPRITES)
    {
        spr->id = sSpriteCount;
        sSprites[sSpriteCount] = spr;
        sSpriteCount++;
        return true;
    }
    LOG_ERR("Sprite: Add() failed, not enough space");
    return false;
}

bool Sprite_Clear()
{
    sSpriteCount = 0;
    for(uint32_t i = 0; i < MAX_SPRITES; i++)
    {
        sSprites[i] = 0;
    }
    return true;
}

int Sprite_CompareFunction(const void * a, const void * b) {
    Sprite spriteA = *(Sprite*)a;
    Sprite spriteB = *(Sprite*)b;
    return spriteA.zOrder - spriteB.zOrder;
}

void Sprite_Update()
{
    for(uint32_t i = 0; i < sSpriteCount; i++)
    {
        qsort(sSprites, sSpriteCount, sizeof(Sprite), Sprite_CompareFunction);
    }   
    for(uint32_t i = 0; i < sSpriteCount; i++)
    {
        if(!sSprites[i]->isVisible || !sSprites[i]->currentTexture)
        {
            continue;
        }
        Vector2 position = {0.0f,0.0f};
        float scale = 0.0f;
        float rotation = 0.0f;
        if(sSprites[i]->parent)
        {
            position.x = sSprites[i]->parent->position.x;
            position.y = sSprites[i]->parent->position.y;
            scale = sSprites[i]->parent->scale;
            rotation = sSprites[i]->parent->rotation;
        }
        position.x += sSprites[i]->position.x;
        position.y += sSprites[i]->position.y;
        scale *= sSprites[i]->scale;
        rotation += sSprites[i]->rotation;
        //TODO: may have a bug here, left because I can loollolo
        Color color;
        color.a = 255;
        color.r = 255;
        color.g = 255;
        color.b = 255;
        // DrawTextureV(*sSprites[i]->currentTexture, pos, color);
        DrawTextureEx(*sSprites[i]->currentTexture, position, rotation, scale, color);
    }
}

uint8_t Sprite_GetCount()
{
    return sSpriteCount;
}

Sprite ** Sprite_GetSprites()
{
    return sSprites;
}

Updatable * Sprite_GetUpdatable()
{
    return &spriteUpdatable;
}