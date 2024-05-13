#ifndef LIBS_ENGINE_SPRITE_H
#define LIBS_ENGINE_SPRITE_H
#include <stdint.h>
#include <stdlib.h>
#include "raylib.h"
#include "utils.h"
#include "engine/components/Entity2D.h"
#include "engine/context/Updatable.h"
#define MAX_SPRITES 32

typedef struct Sprite
{
    Entity2D * parent;
    Vector2 position;
    float scale;
    float rotation;
    uint8_t id;
    Texture2D * currentTexture;
    uint8_t zOrder;
    bool isVisible;
} Sprite;

void Sprite_Initialize(Sprite * spr);
bool Sprite_Add(Sprite * spr);
bool Sprite_Clear();
void Sprite_Update();

uint8_t Sprite_GetCount();
Sprite ** Sprite_GetSprites();
Updatable * Sprite_GetUpdatable();
#endif