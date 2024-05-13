#ifndef LIBS_ENGINE_COLLIDER2D_H
#define LIBS_ENGINE_COLLIDER2D_H
#include <stdint.h>
#include <stdbool.h>
#include "raylib.h"
#include "utils.h"
#include "engine/components/Entity2D.h"
#include "engine/context/Updatable.h"
#define MAX_COLLIDERS 16
#define MAX_COLLISIONS 16
#define SIMPLE_CHECK

typedef struct Collider2D Collider2D;

typedef struct Collision2D
{
    Collider2D * collision[MAX_COLLIDERS];
    uint8_t collisionCount;
} Collision2D;

typedef struct Collider2D
{
    Entity2D * parent;
    Vector2 position;
    Vector2 size;
    bool isEnabled;
    uint8_t id;
    Collision2D collision;
} Collider2D;

void Collider2D_Initialize(Collider2D * col);
bool Collider2D_Add(Collider2D * col);
bool Collider2D_Clear();
void Collider2D_Update();
bool Collider2D_Check(Collider2D * a, Collider2D * b);

uint8_t Collider2D_GetCount();
Collider2D ** Collider2D_GetColliders();
Updatable * Collider2D_GetUpdatable();
#endif