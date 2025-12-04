#ifndef LIBS_ENGINE_SPRITE_H
#define LIBS_ENGINE_SPRITE_H
#include "engine/components/Entity2D.h"
#include "engine/context/Updatable.h"
#include "raylib.h"

#include <stdint.h>
#include <stdlib.h>

typedef struct Sprite
{
    Entity2D*  parent;
    Vector2    position;
    float      scale;
    float      rotation;
    uint8_t    id;
    Texture2D* currentTexture;
    uint8_t    zOrder;
    bool       isVisible;
    Color      tint;
    // Sprite*    next;
} Sprite;

void Sprite_SetPool(Sprite* pool, size_t poolSize);
void Sprite_Initialize(Sprite* spr);
bool Sprite_Add(Sprite* spr);
bool Sprite_Clear();
void Sprite_Update();
void Sprite_Draw(Sprite* spr);

uint32_t   Sprite_GetCount();
Sprite*    Sprite_GetSpriteList();
Updatable* Sprite_GetUpdatable();
#endif
