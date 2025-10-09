#ifndef LIBS_ENGINE_COLLIDER2D_H
#define LIBS_ENGINE_COLLIDER2D_H
#include "engine/components/Entity2D.h"
#include "engine/context/Updatable.h"
#include "raylib.h"

#include <stdbool.h>
#include <stdint.h>
#define COLLIDER2D_MAX_COUNT      16
#define COLLIDER2D_MAX_COLLISIONS 16
#define COLLIDER2D_SIMPLE_CHECK   true

typedef struct Collider2D Collider2D;

typedef struct Collision2D
{
    Collider2D* collision[COLLIDER2D_MAX_COUNT];
    uint8_t     collisionCount;
} Collision2D;

typedef struct Collider2D
{
    Entity2D*   parent;
    Vector2     position;
    Vector2     size;
    bool        isEnabled;
    uint8_t     id;
    Collision2D collision;
    Collider2D* next;
} Collider2D;

void Collider2D_Initialize(Collider2D* col);
bool Collider2D_Add(Collider2D* col);
bool Collider2D_Clear();
void Collider2D_Update();
bool Collider2D_Check(Collider2D* a, Collider2D* b);
bool Collider2D_CheckCollider(Collider2D* a, Collider2D* b);
bool Collider2D_CheckPoint(Collider2D* a, Vector2 b);
bool Collider2D_CheckRect(Collider2D* a, Rectangle b);

uint32_t    Collider2D_GetCount();
Collider2D* Collider2D_GetCollider2DList();
Updatable*  Collider2D_GetUpdatable();
#endif
