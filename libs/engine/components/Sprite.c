#include "Sprite.h"

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
    spr->tint           = WHITE;
    spr->parent         = NULL;
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
        Sprite_Draw(current);
        current = current->next;
    }
}

void Sprite_Draw(Sprite* spr)
{
    if (!spr->isVisible || !spr->currentTexture)
    {
        return;
    }
    Vector2 position = { 0.0f, 0.0f };
    float   scale    = 1.0f;
    float   rotation = 0.0f;
    if (spr->parent != NULL)
    {
        position.x = spr->parent->position.x;
        position.y = spr->parent->position.y;
        scale      = spr->parent->scale;
        rotation   = spr->parent->rotation;
    }
    position.x += spr->position.x * scale;
    position.y += spr->position.y * scale;
    scale *= spr->scale;
    rotation += spr->rotation;
    DrawTextureEx(*spr->currentTexture, position, rotation, scale, spr->tint);
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
