#include "Sprite.h"

#include "engine/misc/Logger.h"
Updatable spriteUpdatable = { Sprite_Update };
uint32_t  sSpriteCount    = 0;
Sprite*   sSpriteList     = NULL;

void Sprite_Initialize(Sprite* spr)
{
    spr->currentTexture = NULL;
    spr->id             = 0;
    spr->position.x     = 0;
    spr->position.y     = 0;
    spr->isVisible      = true;
    spr->rotation       = 0;
    spr->scale          = 1.0f;
    spr->zOrder         = 0;
    spr->next           = NULL;
}

bool Sprite_Add(Sprite* spr)
{
    if (sSpriteCount == 0)
    {
        sSpriteList       = spr;
        sSpriteList->next = NULL;
    }
    else
    {
        Sprite* current = sSpriteList;
        while (current->next != NULL)
        {
            current = current->next;
        }
        current->next = spr;
        spr->next     = NULL;
    }
    sSpriteCount++;
    return true;
}

bool Sprite_Clear()
{
    sSpriteCount = 0;
    sSpriteList  = NULL;
    return true;
}

int Sprite_CompareFunction(const void* a, const void* b)
{
    Sprite spriteA = *(Sprite*)a;
    Sprite spriteB = *(Sprite*)b;
    return spriteA.zOrder - spriteB.zOrder;
}

void Sprite_Update()
{
    // for(uint32_t i = 0; i < sSpriteCount; i++)
    // {
    //     qsort(sSprites, sSpriteCount, sizeof(Sprite), Sprite_CompareFunction);
    // }
    Sprite* current = sSpriteList;
    while (current != NULL)
    {
        if (!current->isVisible || !current->currentTexture)
        {
            current = current->next;
            continue;
        }
        Vector2 position = { 0.0f, 0.0f };
        float   scale    = 1.0f;
        float   rotation = 0.0f;
        if (current->parent)
        {
            position.x = current->parent->position.x;
            position.y = current->parent->position.y;
            scale      = current->parent->scale;
            rotation   = current->parent->rotation;
        }
        position.x += current->position.x;
        position.y += current->position.y;
        scale *= current->scale;
        rotation += current->rotation;
        // TODO: may have a bug here, left because I can loollolo
        Color color;
        color.a = 255;
        color.r = 255;
        color.g = 255;
        color.b = 255;
        DrawTextureEx(*current->currentTexture, position, rotation, scale, color);
        current = current->next;
    }
}

uint32_t Sprite_GetCount()
{
    return sSpriteCount;
}

Sprite* Sprite_GetSpriteList()
{
    return sSpriteList;
}

Updatable* Sprite_GetUpdatable()
{
    return &spriteUpdatable;
}
