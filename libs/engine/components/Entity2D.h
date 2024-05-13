#ifndef LIBS_ENGINE_ENTITY2D_H
#define LIBS_ENGINE_ENTITY2D_H
#include <stdint.h>
#include "raylib.h"
#include "utils.h"
#define MAX_ENTITIES 32

typedef struct Entity2D
{
    Vector2 position;
    float scale;
    float rotation;
    uint8_t id;
} Entity2D;

void Entity2D_Initialize(Entity2D * ent);
bool Entity2D_Add(Entity2D * ent);
bool Entity2D_Clear();

uint8_t Entitiy2D_GetCount();
Entity2D ** Entitiy2D_GetEntities();
#endif