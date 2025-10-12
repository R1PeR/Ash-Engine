#ifndef GAME_GAMEOBJECT_H
#define GAME_GAMEOBJECT_H
#include "raylib.h"
#include "libs/engine/misc/Utils.h"

#include <stdint.h>

enum GameObjectTag
{
    GameObjectNone = 0,
    GameObjectFoliage,
    GameObjectEntity,
    GameObjectProjectile,
    GameObjectEffect
};

typedef struct GameObject
{
    Vector2Int position;
    uint32_t   tag;
    uint8_t    textureId;
    Color      color;
} GameObject;

void GameObject_Initialize(GameObject* ent);
void GameObject_Update(GameObject* gameObject);
#endif
